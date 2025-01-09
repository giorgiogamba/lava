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
