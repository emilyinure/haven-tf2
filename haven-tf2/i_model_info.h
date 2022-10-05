#pragma once
struct mstudiobbox_t {
	int					bone;
	int					group;				// intersection group
	vector				bbmin;				// bounding box
	vector				bbmax;
	int					szhitboxnameindex;	// offset to the name of the hitbox.
	int					unused[ 8 ];

	const char* pszHitboxName( ) {
		if ( szhitboxnameindex == 0 )
			return "";

		return ( ( const char* )this ) + szhitboxnameindex;
	}

	mstudiobbox_t( ) {}

private:
	// No copy constructors allowed
	mstudiobbox_t( const mstudiobbox_t& vOther );
};

struct mstudiohitboxset_t {
	int					sznameindex;
	inline char* const	pszName( void ) const { return ( ( char* )this ) + sznameindex; }
	int					numhitboxes;
	int					hitboxindex;
	inline mstudiobbox_t* pHitbox( int i ) const { return ( mstudiobbox_t* )( ( ( byte* )this ) + hitboxindex ) + i; };
};

struct mstudiobone_t {
	int					sznameindex;
	inline char* const pszName( void ) const { return ( ( char* )this ) + sznameindex; }
	int		 			parent;		// parent bone
	int					bonecontroller[ 6 ];	// bone controller index, -1 == none

											// default values
	vector				pos;
	float				quat[ 4 ];
	vector				rot;
	// compression scale
	vector				posscale;
	vector				rotscale;

	float				poseToBone[ 3 ][ 4 ];
	float				qAlignment[ 4 ];
	int					flags;
	int					proctype;
	int					procindex;		// procedural rule
	mutable int			physicsbone;	// index into physically simulated bone
	inline void* pProcedure( ) const { if ( procindex == 0 ) return NULL; else return  ( void* )( ( ( byte* )this ) + procindex ); };
	int					surfacepropidx;	// index into string tablefor property name
	inline char* const pszSurfaceProp( void ) const { return ( ( char* )this ) + surfacepropidx; }
	int					contents;		// See BSPFlags.h for the contents flags

	int					unused[ 8 ];		// remove as appropriate

	mstudiobone_t( ) {}
private:
	// No copy constructors allowed
	mstudiobone_t( const mstudiobone_t& vOther );
};

struct studiohdr_t {
	int                                     id;
	int                                     version;

	int                                     checksum;

	char                            name[ 64 ];
	int                                     length;


	vector                          eyeposition;

	vector                          illumposition;

	vector                          hull_min;
	vector                          hull_max;

	vector                          view_bbmin;
	vector                          view_bbmax;

	int                                     flags;

	int                                     numbones;
	int                                     boneindex;

	inline mstudiobone_t* GetBone( int i ) const { return ( mstudiobone_t* )( ( ( byte* )this ) + boneindex ) + i; };
	//	inline mstudiobone_t *pBone(int i) const { Assert(i >= 0 && i < numbones); return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };

	int                                     numbonecontrollers;
	int                                     bonecontrollerindex;

	int                                     numhitboxsets;
	int                                     hitboxsetindex;

	mstudiohitboxset_t* GetHitboxSet( int i ) const {
		return ( mstudiohitboxset_t* )( ( ( byte* )this ) + hitboxsetindex ) + i;
	}

	inline mstudiobbox_t* GetHitbox( int i, int set ) const {
		mstudiohitboxset_t const* s = GetHitboxSet( set );

		if ( !s )
			return NULL;

		return s->pHitbox( i );
	}

	inline int GetHitboxCount( int set ) const {
		mstudiohitboxset_t const* s = GetHitboxSet( set );

		if ( !s )
			return 0;

		return s->numhitboxes;
	}

	int                                     numlocalanim;
	int                                     localanimindex;

	int                                     numlocalseq;
	int                                     localseqindex;

	mutable int                     activitylistversion;
	mutable int                     eventsindexed;

	int                                     numtextures;
	int                                     textureindex;

	int                                     numcdtextures;
	int                                     cdtextureindex;

	int                                     numskinref;
	int                                     numskinfamilies;
	int                                     skinindex;

	int                                     numbodyparts;
	int                                     bodypartindex;

	int                                     numlocalattachments;
	int                                     localattachmentindex;

	int                                     numlocalnodes;
	int                                     localnodeindex;
	int                                     localnodenameindex;

	int                                     numflexdesc;
	int                                     flexdescindex;

	int                                     numflexcontrollers;
	int                                     flexcontrollerindex;

	int                                     numflexrules;
	int                                     flexruleindex;

	int                                     numikchains;
	int                                     ikchainindex;

	int                                     nummouths;
	int                                     mouthindex;

	int                                     numlocalposeparameters;
	int                                     localposeparamindex;

	int                                     surfacepropindex;

	int                                     keyvalueindex;
	int                                     keyvaluesize;


	int                                     numlocalikautoplaylocks;
	int                                     localikautoplaylockindex;

	float                           mass;
	int                                     contents;

	int                                     numincludemodels;
	int                                     includemodelindex;

	mutable void* virtualModel;

	int                                     szanimblocknameindex;
	int                                     numanimblocks;
	int                                     animblockindex;

	mutable void* animblockModel;

	int                                     bonetablebynameindex;

	void* pVertexBase;
	void* pIndexBase;

	byte                            constdirectionallightdot;

	byte                            rootLOD;

	byte                            numAllowedRootLODs;

	byte                            unused[ 1 ];

	int                                     unused4;

	int                                     numflexcontrollerui;
	int                                     flexcontrolleruiindex;
	float                           flVertAnimFixedPointScale;
	int                                     unused3[ 1 ];
	int                                     studiohdr2index;
	int                                     unused2[ 1 ];
};
class i_model_info {
public:
	model_t* GetModel( int index ) {
		typedef model_t* ( __thiscall* GetModelFn )( void*, int );
		return g_utils.get_virtual_function<GetModelFn>( this, 1 )( this, index );
	}

	int	GetModelIndex( const char* name ) {
		typedef int( __thiscall* GetModelIndexFn )( void*, const char* );
		return g_utils.get_virtual_function< GetModelIndexFn >( this, 2 )( this, name );
	}

	const char* GetModelName( const model_t* model ) {
		typedef const char* ( __thiscall* GetModelNameFn )( void*, const model_t* );
		return g_utils.get_virtual_function< GetModelNameFn >( this, 3 )( this, model );
	}

	studiohdr_t* GetStudiomodel( const model_t* mod ) {
		typedef studiohdr_t* ( __thiscall* GetStudiomodelFn )( void*, const model_t* );
		return g_utils.get_virtual_function< GetStudiomodelFn >( this, 28 )( this, mod );
	}
};