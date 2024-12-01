#version 430

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(std430, binding = 1) readonly restrict buffer golLayout {
    uint golBuffer[];
};

layout(std430, binding = 2) buffer golLayout2 {
    uint golBufferDest[];
};

layout(std430, binding = 3) buffer cellFlag {
    uint flag;
};

uniform uint gol_width;
uniform uint gol_height;

#define lookup_state(x, y) ((((x) < 0) || ((y) < 0) || ((x) > gol_width) || ((y) > gol_height)) ? (0) \
    : golBuffer[(x) + gol_width * (y)])

#define set_state(x, y, value) golBufferDest[(x) + gol_width * (y)] = value

void main()
{
    uint nCount = 0;
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint index = y * gol_width + x;

    nCount += lookup_state(x - 1, y - 1);
    nCount += lookup_state(x, y - 1);
    nCount += lookup_state(x + 1, y - 1);
    nCount += lookup_state(x - 1, y);
    nCount += lookup_state(x + 1, y);
    nCount += lookup_state(x - 1, y + 1);
    nCount += lookup_state(x, y + 1);
    nCount += lookup_state(x + 1, y + 1);

    if (nCount == 3) set_state(x, y, 1);
    else if (nCount == 2) set_state(x, y, lookup_state(x, y));
    else set_state(x, y, 0);

    if (x == 251 && y == 142 && lookup_state(x, y) > 0) flag = 1;
}
