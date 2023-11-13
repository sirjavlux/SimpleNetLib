#pragma once

#define USE_VIDEO // If this is defined, you can play a video by using Video

#define CAN_USE_DDS_NOT_POWER_OF_TWO true
#define CAN_USE_OTHER_FORMATS_THAN_DDS true 

#define FONT_BUFF_SIZE 1024
#define MAX_TEXT_SPRITES 1024

#ifndef MAX_POINTS_IN_CUSTOM_SHAPE
#define MAX_POINTS_IN_CUSTOM_SHAPE  69984 
#endif


#define SPINE_MESH_VERTEX_COUNT_MAX 1000

// 1 is the baked framerate from maya, if you want to slow down the animation, you might need more precision. Be aware, loadingtimes and memory will suffer if high
// Increase small numbers, try with 3 if you are not happy with "chugging" animations
#define ANIMATION_PRECISION 2

#define MAX_MESHES_PER_MODEL 32

// do not change these without updating common.hlsli
#define NUMBER_OF_LIGHTS_ALLOWED 8
#define MAX_ANIMATION_BONES 64
#define USE_NOISE // If this is defined, a texture with perlin noise will be avalible in all shaders to play with
#define USE_LIGHTS // If this is defined, the engine will calculate per pixel lightning using Light

#define MAX_MODEL_INSTANCES 64
