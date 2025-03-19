#  Notes

## Push constants

Currently, the command buffers collects data in the following way:
1. Starts render pass
2. Binds graphics pipeline
3. Binds model -> bind vertex buffer data
4. Draws model
5. Ends render pass

We can use push constants to pass small amount of data to shaders, and use them to update the command buffer data before ending a draw call
This way, we can draw multiple instances of the same object in a efficient way
NOTE: all the vertices must share the same push constants data

They have limited size of 128 bytes usually -> they must be shared between all shaders

Push Constant Range:
- stage flags -> which stage flags will have access
- offset
- size


By now, the transforms have been represented by 2 matrices (rotation and scaling) and a offset (translation)
We cannot represent the translation as a matrix itself because it cannot maintain the property that the
object has origin in (0, 0). We can do it by using a matrix with 1 more space in both dimensions (we use 3x3 matrix for 2x2 transform)
This is called homogeneous coordinate

Note that if we put the third element to 1, then it is copied inside the resulting matrix -> AFFINE transformation

The main disadvantage is that the translation is always applied as final step, because it is an offset in formula ax + by + c (linear transformation)

We note that relative offsets and distances are affected by Linear Transformations -> this means that, it I compute the lenght of the side of a square,
if I then scale the square, then the side won't be the same -> it changes
At the same time, they are not affected by translations -> the side of a square remains the same also if I move it somewhere else

To make this distinction, we use the third coordinate = 1 if we are representing a point
We use 0 if we are representing a distance/offset

Elemental rotation: rotation about 1 axis of a 3D space
Composing 3 elemental rotations, we can get every target position that we want

Proper Euler Angles: 12 possible combinations of elemental rotations -> THE FIRST AND THIRD ROTATION ARE ABOUT THE SAME AXIS
Tait-Bryan Angles: 12 possible combinations of elemental rotations around 3 different axis

Every combination is equally valid to reach the desired target

Quaternions: alternative way to represent rotations, instead of matrices. It permits to solve some problems:
- Rotations concatenation is faster
- easier to extract angle and rotation axis
- easier interpolation
- no Gimbal lock

Intrinsic cs Estrinsic rotation
Extrinsic rotations: axis X Y Z are not moving while the object is rotating
Intrincis rotations: Axis are attached to the moving object and moving with it

Rotations can be interpreted depending on the way we read axis: 
Given YXZ, if reading from left to right then intrinsic, if reading from right to left then extrinsic.
About intrinsic, this is true because we are doing a rotation around a first axis, then we apply the new rotation to the axis
resulting from the previous operations, meaning that the axis are moving with the object
About extrinsic, we are applying from right to left and thus we are using the fixed axis, with the rotation not applied on the result of the previous rotation


Othographic projection: generalization of a view volume: axis aligned bounding box, with any location and any scale
Defined by 6 planes: near/far, top/bottom, left/right
The challenge is to convert this box to the vulkan canonical view volume: we scale and translate
Ofc the implementation depends on the axis adopted and their direction

Normals computation influences the way the objects are rendered:
- Vertex Normals: they provide a smooth shading and so curved surfaces
- Face Normals: they provide Flat shading -> low poly style -> we can have multiple normals for each vertex position

Uniform Buffers
Uniform buffers are based on minium offset alignment -> supposed is i set to a certain number, we will need the uniform buffer to be of size equal to a multiple of it

Non-coherent-Atom-size: smallest memory range the device allows when syncing between host and device memory -> when executing flushIndex we get an error because of misalignment

Descriptor Sets: Pointer to a resource
E.g.: Image Descriptors: pointer to image and sampler

They store additional data to describe the resource

Vulkan has the advantage, respect to other pipelines, that descriptors must be must be contained inside a sets, based on their binding frequency, before sending to the pipeline for performance reasons

e.g. Camera and perspective matrix are in the same set

Once we bound a descriptor set, we can keep it bound in order to reuse it if resources remain the same

ONLY 4 MAX DESCRIPTOR SETS

The descriptor layout defines how the pipeline must receive descriptor sets, and it is priorly defined

We can define a descriptor pools that contains all sets (used or not) and then bind them at runtime when needed

Point Lights:
Rays are starting from a single point, thus the resulting angles hitting object's surface will be different depending on the point where the hit happens. Intensity is attenuated through the inverse square law, composed in the denominator as (constant + linear * distance + quadratic * distanceˆ2)

Fragment shading:
we want to execute shading on fragments instead of vertices because with vertices and big objects, like for example the floor, the shading is executed on vertices and thus the rest of the mesh is interpolated, resulting in a wrong lighting. Fragment shading is expensive but we can afford it.
