#include "main.h"

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	okapi::Controller master(okapi::ControllerId::master);

	//Drive Train Def

	okapi::Motor FrontRight(1, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees);
	okapi::Motor CenterRight(2, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees);
	okapi::Motor BackRight(3, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees);

	okapi::Motor FrontLeft(11, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees);
	okapi::Motor CenterLeft(12, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees);
	okapi::Motor BackLeft(13, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees);

	okapi::MotorGroup driveTrainLeft({FrontLeft, CenterRight, BackRight});
	okapi::MotorGroup driveTrainRight({FrontRight, CenterRight, BackRight});

	//Arm Def

	okapi::Motor ArmRight(4, true, okapi::AbstractMotor::gearset::green);
	okapi::Motor ArmLeft(5, false, okapi::AbstractMotor::gearset::green);

	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);
		
		int left = master.getAnalog(okapi::ControllerAnalog::leftY);
		int right = master.getAnalog(okapi::ControllerAnalog::rightY);

		driveTrainLeft.moveVelocity(left*600);
		driveTrainRight.moveVelocity(right*600);

		bool rightArmUp = master.getDigital(okapi::ControllerDigital::R1);
		bool rightArmDown = master.getDigital(okapi::ControllerDigital::R2);

		bool leftArmUp = master.getDigital(okapi::ControllerDigital::L1);
		bool leftArmDown = master.getDigital(okapi::ControllerDigital::L2);

		if(rightArmUp)
			ArmRight.moveVelocity(100);
		else if(rightArmDown)
			ArmRight.moveVelocity(-100);

		if(leftArmUp)
			ArmLeft.moveVelocity(100);
		else if(leftArmDown)
			ArmLeft.moveVelocity(-100);

		pros::delay(20);
	}
}
