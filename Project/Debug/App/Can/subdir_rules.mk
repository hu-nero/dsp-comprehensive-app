################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
App/Can/can_app.obj: ../App/Can/can_app.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"D:/ccs/ccsv6/tools/compiler/ti-cgt-c2000_22.6.2.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="D:/ccs/ccsv6/tools/compiler/ti-cgt-c2000_22.6.2.LTS/include" --include_path="D:/workspaces/MPR_500kW/DSP28335_Project/Project" --include_path="D:/workspaces/MPR_500kW/DSP28335_Project/Project/App" --include_path="D:/workspaces/MPR_500kW/DSP28335_Project/Project/Common" --include_path="D:/workspaces/MPR_500kW/DSP28335_Project/Project/SDK/Agent" --include_path="D:/workspaces/MPR_500kW/DSP28335_Project/Project/SDK/Drivers" --include_path="D:/workspaces/MPR_500kW/DSP28335_Project/Project/SDK/Hal" --include_path="D:/workspaces/MPR_500kW/DSP28335_Project/Project/Services/Adapter" --include_path="D:/workspaces/MPR_500kW/DSP28335_Project/Project/Services/Logic" --include_path="D:/workspaces/MPR_500kW/DSP2833x_headers/include" --include_path="D:/workspaces/MPR_500kW/DSP2833x_common/include" --include_path="D:/workspaces/MPR_500kW/DSP28335_Project/source" --advice:performance=all -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="App/Can/can_app.d" --obj_directory="App/Can" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


