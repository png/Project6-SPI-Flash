################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/opt/ti/ccs910/ccs/tools/compiler/msp430-gcc-8.2.0.52_linux64/bin/msp430-elf-gcc" -c -mmcu=msp430g2553 -mhwmult=none -I"/opt/ti/ccs910/ccs/ccs_base/msp430/include_gcc" -I"/home/jason/Documents/CourseWork/ECE3430/workspace_v9/Project6" -I"/opt/ti/ccs910/ccs/tools/compiler/msp430-gcc-8.2.0.52_linux64/msp430-elf/include" -Og -g -gdwarf-3 -gstrict-dwarf -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


