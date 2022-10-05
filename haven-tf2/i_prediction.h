#pragma once

class CMoveData {
public:
	bool m_bFirstRunOfFunctions : 1;
	bool m_bGameCodeMovedPlayer : 1;

	void* m_nPlayerHandle; // edict index on server, client entity handle on client

	int    m_nImpulseCommand; // Impulse command issued.
	vector m_vecViewAngles; // Command view angles (local space)
	vector m_vecAbsViewAngles; // Command view angles (world space)
	int    m_nButtons; // Attack buttons.
	int    m_nOldButtons; // From host_client->oldbuttons;
	float  m_flForwardMove;
	float  m_flSideMove;
	float  m_flUpMove;

	float m_flMaxSpeed;
	float m_flClientMaxSpeed;

	// Variables from the player edict (sv_player) or entvars on the client.
	// These are copied in here before calling and copied out after calling.
	vector m_vecVelocity; // edict::velocity		// Current movement direction.
	vector m_vecAngles; // edict::angles
	vector m_vecOldAngles;

	// Output only
	float  m_outStepHeight; // how much you climbed this move
	vector m_outWishVel; // This is where you tried
	vector m_outJumpVel; // This is your jump velocity

					   // Movement constraints	(radius 0 means no constraint)
	vector m_vecConstraintCenter;
	float  m_flConstraintRadius;
	float  m_flConstraintWidth;
	float  m_flConstraintSpeedFactor;

	void        SetAbsOrigin( const vector& vec );
	const vector& GetAbsOrigin( ) const;

	// private:
	vector m_vecAbsOrigin; // edict::origin
};
class CMoveHelper {
public:
	virtual void SetHost( c_base_entity* host ) = 0;
};
class i_prediction {
public:
	virtual		 ~i_prediction( void ) {};
	virtual void Init( void ) = 0;
	virtual void Shutdown( void ) = 0;
	virtual void Update
	(
		int startframe,
		bool validframe,
		int incoming_acknowledged,
		int outgoing_command
	) = 0;

	virtual void PreEntityPacketReceived( int commands_acknowledged, int current_world_update_packet ) = 0;
	virtual void PostEntityPacketReceived( void ) = 0;
	virtual void PostNetworkDataReceived( int commands_acknowledged ) = 0;
	virtual void OnReceivedUncompressedPacket( void ) = 0;
	virtual void GetViewOrigin( vector& org ) = 0;
	virtual void SetViewOrigin( vector& org ) = 0;
	virtual void GetViewAngles( vector& ang ) = 0;
	virtual void SetViewAngles( vector& ang ) = 0;
	virtual void GetLocalViewAngles( vector& ang ) = 0;
	virtual void SetLocalViewAngles( vector& ang ) = 0;
};
class c_prediction : public i_prediction {
public:
	c_prediction( void );
	virtual		 ~c_prediction( );
	virtual void Init( );
	virtual void Shutdown( );
	virtual void Update
	(
		int startframe,
		bool validframe,
		int incoming_acknowledged,
		int outgoing_command
	);

	virtual void OnReceivedUncompressedPacket( ) = 0;
	virtual void PreEntityPacketReceived( int commands_acknowledged, int current_world_update_packet ) = 0;
	virtual void PostEntityPacketReceived( ) = 0;
	virtual void PostNetworkDataReceived( int commands_acknowledged ) = 0;
	virtual bool InPrediction( ) = 0;
	virtual bool IsFirstTimePredicted( ) = 0;
	virtual int	 GetIncomingPacketNumber( ) = 0;
	virtual void GetViewOrigin( vector& org ) = 0;
	virtual void SetViewOrigin( vector& org ) = 0;
	virtual void GetViewAngles( vector& ang ) = 0;
	virtual void SetViewAngles( vector& ang ) = 0;
	virtual void GetLocalViewAngles( vector& ang ) = 0;
	virtual void SetLocalViewAngles( vector& ang ) = 0;
	virtual void RunCommand( c_base_entity* player, usercmd_t* ucmd, CMoveHelper* moveHelper ) = 0;
	virtual void SetupMove( c_base_entity* player, usercmd_t* ucmd, CMoveHelper* pHelper, CMoveData* move ) = 0;
	virtual void FinishMove( c_base_entity* player, usercmd_t* ucmd, CMoveData* move ) = 0;
	virtual void SetIdealPitch( c_base_entity* player, const vector& origin, const vector& angles, const vector& viewheight ) = 0;
	virtual void _Update
	(
		bool received_new_world_update,
		bool validframe,
		int incoming_acknowledged,
		int outgoing_command
	) = 0;

	//CHandle< C_BaseEntity > m_hLastGround;
	uint32_t m_hLastGround;
	bool	 m_bInPrediction;
	bool	 m_bFirstTimePredicted;
	bool	 m_bOldCLPredictValue;
	bool	 m_bEnginePaused;
	int		 m_nPreviousStartFrame;
	int		 m_nCommandsPredicted;
	int		 m_nServerCommandsAcknowledged;
	int		 m_bPreviousAckHadErrors;
	int		 m_nIncomingPacketNumber;
	float	 m_flIdealPitch;
};
class c_game_movement {
public:
	virtual			~c_game_movement( void ) {}

	// Process the current movement command
	virtual void	ProcessMovement( c_base_entity* pPlayer, CMoveData* pMove ) = 0;
	virtual void	StartTrackPredictionErrors( c_base_entity* pPlayer ) = 0;
	virtual void	FinishTrackPredictionErrors( c_base_entity* pPlayer ) = 0;
	virtual void	DiffPrint( char const* fmt, ... ) = 0;

	// Allows other parts of the engine to find out the normal and ducked player bbox sizes
	virtual vector GetPlayerMins( bool ducked ) const = 0;
	virtual vector GetPlayerMaxs( bool ducked ) const = 0;
	virtual vector GetPlayerViewOffset( bool ducked ) const = 0;
};