#pragma once

// forward decl.
class i_client_unknown;

// todo: remove once impl.
struct trace_t;
struct model_t;

enum solid_type_t {
	SOLID_NONE = 0,
	SOLID_BSP = 1,
	SOLID_BBOX = 2,
	SOLID_OBB = 3,
	SOLID_OBB_YAW = 4,
	SOLID_CUSTOM = 5,
	SOLID_VPHYSICS = 6,
	SOLID_LAST,
};

enum Collision_Group_t
{
	COLLISION_GROUP_NONE = 0,
	COLLISION_GROUP_DEBRIS,			// Collides with nothing but world and static stuff
	COLLISION_GROUP_DEBRIS_TRIGGER, // Same as debris, but hits triggers
	COLLISION_GROUP_INTERACTIVE_DEBRIS,	// Collides with everything except other interactive debris or debris
	COLLISION_GROUP_INTERACTIVE,	// Collides with everything except interactive debris or debris
	COLLISION_GROUP_PLAYER,
	COLLISION_GROUP_BREAKABLE_GLASS,
	COLLISION_GROUP_VEHICLE,
	COLLISION_GROUP_PLAYER_MOVEMENT,  // For HL2, same as Collision_Group_Player, for
										// TF2, this filters out other players and CBaseObjects
										COLLISION_GROUP_NPC,			// Generic NPC group
										COLLISION_GROUP_IN_VEHICLE,		// for any entity inside a vehicle
										COLLISION_GROUP_WEAPON,			// for any weapons that need collision detection
										COLLISION_GROUP_VEHICLE_CLIP,	// vehicle clip brush to restrict vehicle movement
										COLLISION_GROUP_PROJECTILE,		// Projectiles!
										COLLISION_GROUP_DOOR_BLOCKER,	// Blocks entities not permitted to get near moving doors
										COLLISION_GROUP_PASSABLE_DOOR,	// Doors that the player shouldn't collide with
										COLLISION_GROUP_DISSOLVING,		// Things that are dissolving are in this group
										COLLISION_GROUP_PUSHAWAY,		// Nonsolid on client and server, pushaway in player code

										COLLISION_GROUP_NPC_ACTOR,		// Used so NPCs in scripts ignore the player.
										COLLISION_GROUP_NPC_SCRIPTED,	// USed for NPCs in scripts that should not collide with each other

										LAST_SHARED_COLLISION_GROUP
};

enum tf_collision_group {
	TF_COLLISIONGROUP_GRENADES = LAST_SHARED_COLLISION_GROUP,
	TFCOLLISION_GROUP_OBJECT,
	TFCOLLISION_GROUP_OBJECT_SOLIDTOPLAYERMOVEMENT,
	TFCOLLISION_GROUP_COMBATOBJECT,
	TFCOLLISION_GROUP_ROCKETS,		// Solid to players, but not player movement. ensures touch calls are originating from rocket
	TFCOLLISION_GROUP_RESPAWNROOMS,
	TFCOLLISION_GROUP_TANK,
	TFCOLLISION_GROUP_ROCKET_BUT_NOT_WITH_OTHER_ROCKETS,

	//
	// ADD NEW ITEMS HERE TO AVOID BREAKING DEMOS
	//
};

class i_collideable {
public:
	virtual i_handle_entity* get_entity_handle( ) = 0;
	virtual const vector& obb_mins_pre_scaled( ) const = 0;
	virtual const vector& obb_maxs_pre_scaled( ) const = 0;
	virtual const vector& obb_mins( ) const = 0;
	virtual const vector& obb_maxs( ) const = 0;
	virtual void world_space_trigger_bounds( vector* vec_world_mins, vector* vec_world_maxs ) const = 0;
	virtual bool test_collision( const ray_t& ray, unsigned int contents_mask, trace_t& tr ) = 0;
	virtual bool test_hitboxes( const ray_t& ray, unsigned int contents_mask, trace_t& tr ) = 0;
	virtual int get_collision_model_index( ) = 0;
	virtual const model_t* get_collision_model( ) = 0;
	virtual const vector& get_collision_origin( ) const = 0;
	virtual const vector& get_collision_angles( ) const = 0;
	virtual const matrix_3x4& collision_to_world_transform( ) const = 0;
	virtual solid_type_t get_solid( ) const = 0;
	virtual int get_solid_flags( ) const = 0;
	virtual i_client_unknown* get_i_client_unknown( ) = 0;
	virtual int get_collision_group( ) const = 0;
	virtual void world_space_surrounding_bounds( vector* vec_mins, vector* vec_maxs ) = 0;
	virtual bool should_touch_trigger( int trigger_solid_flags ) const = 0;
	virtual const matrix_3x4* get_root_parent_to_world_transform( ) const = 0;
};