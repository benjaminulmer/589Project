################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Camera.cpp \
../src/ContentLoading.cpp \
../src/InputHandler.cpp \
../src/Program.cpp \
../src/RenderEngine.cpp \
../src/Renderable.cpp \
../src/ShaderTools.cpp \
../src/Texture.cpp \
../src/lodepng.cpp \
../src/main.cpp 

OBJS += \
./src/Camera.o \
./src/ContentLoading.o \
./src/InputHandler.o \
./src/Program.o \
./src/RenderEngine.o \
./src/Renderable.o \
./src/ShaderTools.o \
./src/Texture.o \
./src/lodepng.o \
./src/main.o 

CPP_DEPS += \
./src/Camera.d \
./src/ContentLoading.d \
./src/InputHandler.d \
./src/Program.d \
./src/RenderEngine.d \
./src/Renderable.d \
./src/ShaderTools.d \
./src/Texture.d \
./src/lodepng.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


