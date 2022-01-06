#pragma once

// refs
class ENGINE_API	IRender_Visual;
class ENGINE_API	CInifile;
class ENGINE_API 	CEnvironment;

// refs - effects
class ENGINE_API	CEnvironment;
class ENGINE_API	CLensFlare;
class ENGINE_API	CEffect_Rain;
class ENGINE_API	CEffect_Thunderbolt;

class ENGINE_API	CPerlinNoise1D;

struct SThunderboltDesc;
struct SThunderboltCollection;
class CLensFlareDescriptor;

#define DAY_LENGTH		86400.f

#include "../Include/xrRender/FactoryPtr.h"
#include "../Include/xrRender/EnvironmentRender.h"

#ifdef INGAME_EDITOR
#	define	INGAME_EDITOR_VIRTUAL	virtual
#else // #ifdef INGAME_EDITOR
#	define	INGAME_EDITOR_VIRTUAL
#endif // #ifdef INGAME_EDITOR

// t-defs
class ENGINE_API	CEnvModifier
{
public:
	Fvector3			position;
	float				radius;
	float				power;

	float				far_plane;
	Fvector3			fog_color;
	float				fog_density;
	Fvector3			ambient;
	Fvector3			sky_color;		
	Fvector3			hemi_color;
	Flags16				use_flags;

	void				load		(IReader* fs, u32 version);
	float				sum			(CEnvModifier&	_another, Fvector3& view);
};

class ENGINE_API	CEnvAmbient
{
public:
	struct SEffect{
		u32 			life_time;
		ref_sound		sound;		
		shared_str		particles;
		Fvector			offset;
		float			wind_gust_factor;
	};
	DEFINE_VECTOR(SEffect, EffectVec, EffectVecIt);
protected:
	shared_str				section;

	EffectVec				effects;
	xr_vector<ref_sound>	sounds;
	Fvector2				sound_dist;
	Ivector2				sound_period;
	Ivector2				effect_period;
public:
	void				load(const shared_str& section);
	IC SEffect*			get_rnd_effect() { return effects.empty() ? 0 : &effects[Random.randI(effects.size())]; }
	IC ref_sound*		get_rnd_sound() { return sounds.empty() ? 0 : &sounds[Random.randI(sounds.size())]; }
	IC const shared_str&name() { return section; }
	IC u32				get_rnd_sound_time() { return Random.randI(sound_period.x, sound_period.y); }
	IC float			get_rnd_sound_dist() { return Random.randF(sound_dist.x, sound_dist.y); }
	IC u32				get_rnd_effect_time() { return Random.randI(effect_period.x, effect_period.y); }
};

class ENGINE_API	CEnvDescriptor
{
public:
	float				exec_time;
	float				exec_time_loaded;

	shared_str			sky_texture_name	;
	shared_str			sky_texture_env_name;
	shared_str			clouds_texture_name	;

	/*
	ref_texture			sky_texture		;
	ref_texture			sky_texture_env	;
	ref_texture			clouds_texture	;
	*/
	FactoryPtr<IEnvDescriptorRender>	m_pDescriptor;

	Fvector4			clouds_color	;
	Fvector3			sky_color		;
	float				sky_rotation	;

	float				far_plane;

	Fvector3			fog_color;
	float				fog_density;
	float				fog_distance;

	float				rain_density;
	Fvector3			rain_color;

	float				bolt_period;
	float				bolt_duration;

    float				wind_velocity;
    float				wind_direction;  
    
	Fvector3			ambient		;
	Fvector4			hemi_color	;	// w = R2 correction
	Fvector3			sun_color	;
	Fvector3			sun_dir		;
	float				m_fSunShaftsIntensity;
	float				m_fWaterIntensity;


//	int					lens_flare_id;
//	int					tb_id;
	shared_str			lens_flare_id;
	shared_str			tb_id;
    
	CEnvAmbient*		env_ambient;

	CEnvDescriptor();
	void load(CEnvironment& environment, LPCSTR exec_tm, LPCSTR S);
	void load(float exec_tm, LPCSTR S, CEnvironment& environment);
	void				copy			(const CEnvDescriptor& src)
	{
		float tm0		= exec_time;
		float tm1		= exec_time_loaded; 
		*this			= src;
		exec_time		= tm0;
		exec_time_loaded= tm1;
	}

	void				on_device_create	();
	void				on_device_destroy	();

	shared_str			m_identifier;
};

class ENGINE_API		CEnvDescriptorMixer: public CEnvDescriptor{
public:
	/*
	STextureList		sky_r_textures;		
	STextureList		sky_r_textures_env;	
	STextureList		clouds_r_textures;	
	*/
	FactoryPtr<IEnvDescriptorMixerRender>	m_pDescriptorMixer;
	float				weight;				

	float				fog_near;		
	float				fog_far;		
public:
						CEnvDescriptorMixer	(shared_str const& identifier);
	INGAME_EDITOR_VIRTUAL void lerp			(CEnvironment* parent, CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& M, float m_power);
	void				clear				();
	void				destroy				();
};

class ENGINE_API	CEnvironment
{
	friend class dxEnvironmentRender;
	struct str_pred : public std::binary_function<shared_str, shared_str, bool>	{	
		IC bool operator()(const shared_str& x, const shared_str& y) const
		{	return xr_strcmp(x,y)<0;	}
	};
public:
	DEFINE_VECTOR			(CEnvAmbient*,EnvAmbVec,EnvAmbVecIt);
	DEFINE_VECTOR			(CEnvDescriptor*,EnvVec,EnvIt);
	DEFINE_MAP_PRED			(shared_str,EnvVec,EnvsMap,EnvsMapIt,str_pred);
private:
	// clouds
	FvectorVec				CloudsVerts;
	U16Vec					CloudsIndices;
private:
	float					NormalizeTime	(float tm);
	float					TimeDiff		(float prev, float cur);
	float					TimeWeight		(float val, float min_t, float max_t);
	void					SelectEnvs		(EnvVec* envs, CEnvDescriptor*& e0, CEnvDescriptor*& e1, float tm);
	void					SelectEnv		(EnvVec* envs, CEnvDescriptor*& e, float tm);
	void					StopWFX			();

	void					calculate_dynamic_sun_dir();
public:
	static bool sort_env_pred	(const CEnvDescriptor* x, const CEnvDescriptor* y)
	{	return x->exec_time < y->exec_time;	}
	static bool sort_env_etl_pred	(const CEnvDescriptor* x, const CEnvDescriptor* y)
	{	return x->exec_time_loaded < y->exec_time_loaded;	}
protected:
	CPerlinNoise1D*			PerlinNoise1D;

	float					fGameTime;
public:
	FactoryPtr<IEnvironmentRender>	m_pRender;
	BOOL					bNeed_re_create_env;

	float					wind_strength_factor;	
	float					wind_gust_factor;

	// wind blast params
	float					wind_blast_strength;
	Fvector					wind_blast_direction;
	Fquaternion				wind_blast_start_time;
	Fquaternion				wind_blast_stop_time;
	float					wind_blast_strength_start_value;
	float					wind_blast_strength_stop_value;
	Fquaternion				wind_blast_current;
	// Environments
	CEnvDescriptorMixer*	CurrentEnv;
	CEnvDescriptor*			Current[2];

	bool					bWFX;
	float					wfx_time;
	CEnvDescriptor*			WFX_end_desc[2];
    
    EnvVec*					CurrentWeather;
    shared_str				CurrentWeatherName;
	shared_str				CurrentCycleName;

	EnvsMap					WeatherCycles;
	EnvsMap					WeatherFXs;
	xr_vector<CEnvModifier>	Modifiers;
	EnvAmbVec				Ambients;

	CEffect_Rain*			eff_Rain;
	CLensFlare*				eff_LensFlare;
	CEffect_Thunderbolt*	eff_Thunderbolt;

	float					fTimeFactor;

    void					SelectEnvs			(float gt);

	void					UpdateAmbient		();
	INGAME_EDITOR_VIRTUAL CEnvAmbient* AppendEnvAmb	(const shared_str& sect);

	void					Invalidate			();
public:
							CEnvironment		();

	INGAME_EDITOR_VIRTUAL	~CEnvironment		();

	INGAME_EDITOR_VIRTUAL void	load			();
    INGAME_EDITOR_VIRTUAL void	unload			();

	void					mods_load			();
	void					mods_unload			();

	void					OnFrame				();
	void					lerp				(float& current_weight);

	void					RenderSky			();
	void					RenderClouds		();
	void					RenderFlares		();
	void					RenderLast			();

	bool					SetWeatherFX		(shared_str name);
	bool					IsWFXPlaying		(){return bWFX;}
    void					SetWeather			(shared_str name, bool forced=false);
    shared_str				GetWeather			()					{ return CurrentWeatherName;}
	void					SetGameTime			(float game_time, float time_factor);

	void					OnDeviceCreate		();
	void					OnDeviceDestroy		();


	bool					m_paused;

protected:
	INGAME_EDITOR_VIRTUAL	CEnvDescriptor* create_descriptor	(LPCSTR exec_tm, LPCSTR S);
	INGAME_EDITOR_VIRTUAL	void load_weathers					();
	INGAME_EDITOR_VIRTUAL	void load_weather_effects			();
	INGAME_EDITOR_VIRTUAL	void create_mixer					();
							void destroy_mixer					();


public:
	INGAME_EDITOR_VIRTUAL	SThunderboltDesc* thunderbolt_description		(CInifile* config, shared_str const& section);
	INGAME_EDITOR_VIRTUAL	SThunderboltCollection* thunderbolt_collection	(CInifile* pIni, LPCSTR section);
	INGAME_EDITOR_VIRTUAL	SThunderboltCollection* thunderbolt_collection	(xr_vector<SThunderboltCollection*>& collection,  shared_str const& id);
	INGAME_EDITOR_VIRTUAL	CLensFlareDescriptor*	add_flare				(xr_vector<CLensFlareDescriptor*>& collection, shared_str const& id);

public:
	float						p_var_alt;
	float						p_var_long;
	float						p_min_dist;
	float						p_tilt;
	float						p_second_prop;
	float						p_sky_color;
	float						p_sun_color;
	float						p_fog_color;
};

#undef INGAME_EDITOR_VIRTUAL

ENGINE_API extern Flags32	psEnvFlags;
ENGINE_API extern float		psVisDistance;
