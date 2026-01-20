#include "cbase.h"
#include "props.h"
#include "items.h"
#include "datacache/imdlcache.h"

// Define the model paths here. Change these to match your actual .mdl files.
#define ENTITY_MODEL        "models/cubes/big_cube.mdl"
#define DEBRIS_MODEL        "models/cubes/mini_cube.mdl"

// The health of the cube
#define CUBE_HEALTH         100

class CBreakableCube : public CBaseAnimating
{
public:
    DECLARE_CLASS( CBreakableCube, CBaseAnimating );
    DECLARE_DATADESC();

    void    Spawn( void );
    void    Precache( void );
    
    // Override Event_Killed to handle the breaking logic
    void    Event_Killed( const CTakeDamageInfo &info );

private:
    void    CreateDebris( const Vector &velocity );
};

LINK_ENTITY_TO_CLASS( ent_breakable_cube, CBreakableCube );

// Start of Data Description for Save/Load
BEGIN_DATADESC( CBreakableCube )
    // CBaseAnimating handles most fields, we can add custom ones here if needed
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Precache assets
//-----------------------------------------------------------------------------
void CBreakableCube::Precache( void )
{
    PrecacheModel( ENTITY_MODEL );
    PrecacheModel( DEBRIS_MODEL );

    BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Initialize the entity
//-----------------------------------------------------------------------------
void CBreakableCube::Spawn( void )
{
    Precache();

    SetModel( ENTITY_MODEL );
    
    // Set the entity to be solid and use the model's bounding box
    SetSolid( SOLID_VPHYSICS );
    SetMoveType( MOVETYPE_NONE ); // Acts like prop_dynamic (static until broken)

    // Create VPhysics object (requires the model to have a collision mesh)
    if ( !VPhysicsInitNormal( SOLID_VPHYSICS, 0, false ) )
    {
        SetSolid( SOLID_BBOX );
        AddSolidFlags( FSOLID_NOT_SOLID );
    }

    // Set Health and allow it to take damage
    SetHealth( CUBE_HEALTH );
    m_takedamage = DAMAGE_YES;

    BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Called when health <= 0
//-----------------------------------------------------------------------------
void CBreakableCube::Event_Killed( const CTakeDamageInfo &info )
{
    // 1. Create the 8 sub-cubes
    // Calculate the explosion force based on damage direction
    Vector vecForce = info.GetDamageForce();
    VectorNormalize( vecForce );
    vecForce *= 200.0f; // Add some base velocity

    CreateDebris( vecForce );

    // 2. Add a visual effect (optional, e.g., dust or sound)
    CPASAttenuationFilter filter( this );
    EmitSound( filter, entindex(), "Physics.GlassBreak" );

    // 3. Remove this original entity immediately
    UTIL_Remove( this );
}

//-----------------------------------------------------------------------------
// Purpose: Spawns 8 prop_physics entities in the 8 octants of the original
//-----------------------------------------------------------------------------
void CBreakableCube::CreateDebris( const Vector &pushVelocity )
{
    // Get the bounding box of the main model
    Vector vecMins, vecMaxs;
    GetAliveWorldAlignedMinsAndMaxs( &vecMins, &vecMaxs );

    // Calculate the center point
    Vector vecCenter = GetAbsOrigin();
    
    // Calculate the offset distance for the sub-cubes.
    // Assuming the cube is symmetrical, we want the point halfway between center and corner.
    // If your origin is at the bottom of the model, you may need to adjust Z manually.
    Vector vecSize = vecMaxs - vecMins;
    Vector vecOffset = vecSize * 0.25f; 

    // There are 8 corners in a cube. We iterate through them using a bitmask.
    // 000 (bottom-left-back) to 111 (top-right-front)
    for ( int i = 0; i < 8; i++ )
    {
        Vector spawnPos = vecCenter;

        // X Axis: if bit 0 is set, add offset, else subtract
        spawnPos.x += ( i & 1 ) ? vecOffset.x : -vecOffset.x;
        
        // Y Axis: if bit 1 is set, add offset, else subtract
        spawnPos.y += ( i & 2 ) ? vecOffset.y : -vecOffset.y;

        // Z Axis: if bit 2 is set, add offset, else subtract
        spawnPos.z += ( i & 4 ) ? vecOffset.z : -vecOffset.z;

        // Create the physics prop
        CBaseEntity *pDebris = CreateEntityByName( "prop_physics" );
        if ( pDebris )
        {
            pDebris->SetAbsOrigin( spawnPos );
            pDebris->SetAbsAngles( GetAbsAngles() );
            pDebris->SetModel( DEBRIS_MODEL );
            
            // Spawn it into the world
            pDebris->Spawn();

            // Apply velocity to make them fly apart
            IPhysicsObject *pPhys = pDebris->VPhysicsGetObject();
            if ( pPhys )
            {
                // Calculate an outward vector from the center of the big cube
                Vector vecOutward = spawnPos - vecCenter;
                VectorNormalize( vecOutward );
                
                // Combine the explosion push with the natural outward direction
                Vector finalVel = pushVelocity + ( vecOutward * 300.0f ); // 300 is speed magnitude
                
                pPhys->SetVelocity( &finalVel, NULL );
            }
        }
    }
}
