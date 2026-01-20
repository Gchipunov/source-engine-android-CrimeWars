$modelname "cubes/big_cube.mdl"
$scale 1.0
$body "Body" "cube_ref.smd" // Your 64x64x64 cube mesh

$surfaceprop "concrete" // Sounds like concrete when hit

$contents "solid"

// Only needed if you want an Idle animation
$sequence "idle" "cube_ref.smd" {
    fps 30
    loop
}

// ---------------------------------------------------------
// Physics / Collision Data
// ---------------------------------------------------------
$collisionmodel "cube_phy.smd"
{
    $mass 500.0         // Heavy enough to not get pushed easily
    $inertia 1.00
    $damping 0.00
    $rotdamping 0.00
    $concave            // Use $concave if your mesh isn't perfectly convex
}

// ---------------------------------------------------------
// Keyvalues: Enhancing the break
// ---------------------------------------------------------
// Even though C++ handles the big 8-cube split, we can tell 
// the engine to spawn tiny generic rocks/dust using "prop_data".
$keyvalues
{
    prop_data
    {
        "base"          "Stone.Huge"  // Inherit standard stone properties
        "health"        "0"           // We override this in C++, but 0 ensures it's breakable logic
        "physicsmode"   "0"           // 0 = Normal physics
    }
    
    // Optional: If you wanted the ENGINE to handle gibs instead of C++,
    // you would list them here. Since we use C++, we leave this blank
    // or use it for extra particle dust.
}
