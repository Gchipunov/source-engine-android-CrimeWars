$modelname "cubes/mini_cube.mdl"

// AUTOMATIC SCALING: This makes it 1/8th the volume (1/2 width/height/depth)
$scale 0.5 

$body "Body" "cube_ref.smd"

$surfaceprop "concrete"

$sequence "idle" "cube_ref.smd" fps 30

$collisionmodel "cube_phy.smd"
{
    $mass 40.0         // Much lighter
    $inertia 1.00
    $damping 0.00
    $rotdamping 0.00
}

$keyvalues
{
    prop_data
    {
        "base"        "Stone.Medium"
        "health"      "50"  // The little cubes have 50HP and can break into generic rocks
    }
}
