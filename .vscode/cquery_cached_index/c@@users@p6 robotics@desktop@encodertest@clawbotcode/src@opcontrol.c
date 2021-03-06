/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"
#include "chassis.h"
#include "arm.h"
#include "home.h"
#include "joystickPIDControl.h"
#include "kinematics.h"
#include "stabilizeControl.h"
#include "paths.h"
#include "pathFollower.h"
#include "lineFollower.h"
#include "usefulFunctions.h"



/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */
void operatorControl() {

	Encoder encoderShoulder = encoderInit(8, 9, true);
  Encoder encoderElbow = encoderInit(6, 7, false);
	Ultrasonic ultrasonic = ultrasonicInit(1,2);

	for (int i = 1; i < 4; i++) {
		analogCalibrate(i);
	}

	stabilizeControl(encoderElbow, encoderShoulder, -1);

	home(encoderElbow, encoderShoulder);

	stabilizeControl(encoderElbow, encoderShoulder, -1);

	int x = (int)L1;
	int z = (int)L2-10;

	printf("started PID\n");
	for (int i = 0; i < 24; i+= 1){

		z++;
		printf("x : %d: ", x);
		printf("x_calc : %f: ", forward_kinematic_x((int)upperElbow(x,z ), (int)upperShoulder(x,z )));
		printf("x diff : %f", x - forward_kinematic_x((int)upperElbow(x,z ), (int)upperShoulder(x,z )));
		printf("\n");
		printf("z : %d: ", z);
		printf("z_calc : %f: ", forward_kinematic_z((int)upperElbow(x,z ), (int)upperShoulder(x,z )));
		printf("z diff : %f", z - forward_kinematic_z((int)upperElbow(x,z ), (int)upperShoulder(x,z )));

		printf("\n\n");



		PIDContol(encoderElbow, encoderShoulder, (int)upperElbow(x,z), (int)upperShoulder(x,z));
		stabilizeControl(encoderElbow, encoderShoulder, 2);

	}
	printf("finished PID\n");

	stabilizeControl(encoderElbow, encoderShoulder, -1);

	ultrasonicFollower(encoderElbow, encoderShoulder, ultrasonic);

	delay(1000);

	lineFollower();

	delay(1000);
	
	joystickPIDContol(encoderElbow, encoderShoulder);

	stabilizeControl(encoderElbow, encoderShoulder, -1);



// double timeSearching = 0;
// double turn;
//
//
// while (1) {
//
//
// 		turn = min(0.0001 * timeSearching * sin(5 * timeSearching * M_PI/180), 20);
// 		timeSearching += 1;
// 		chassisSet(0, turn);
// 		delay(20);
//
//
// }

//
// while (true) {
// 	int elbowTarget;
// 	int shoulderTarget;
//
// 	printf("pick new elbow and shoulder values\n" );
// 	// scanf("%d %d", &elbowTarget, &shoulderTarget);
//
// 	PIDContol(encoderElbow, encoderShoulder, elbowTarget, shoulderTarget);
// 	stabilizeControl(encoderElbow, encoderShoulder, 10);
//
// }

	// // while(true) {
	// // 	i ++;
	// 	z += 5;
	// 	if (i%10 == 0) {
	// 		z -= 50;
	// 	}
	// 	elbowTarget = (int)upperElbow(x,z);
	// 	shoulderTarget = (int)upperShoulder(x,z);
	// 	PIDContol(encoderElbow, encoderShoulder, elbowTarget, shoulderTarget);
	// 	printf("%d\n",i);
	// }
}
