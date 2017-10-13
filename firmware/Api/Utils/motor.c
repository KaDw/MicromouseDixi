/*
 * motor.c
 *
 *  Created on: 12.10.2017
 *      Author: Karol Trzcinski
 *
 *
 *  Description:
 *  	module to control 2 motors of mobile robot
 *  	it base on motor_driver, there can be used a lot of sensors
 *  	to find best PWM coefficient
 */

#include "motor.h"

// global instance of motors
motors_t motors;

// global instance of motors callback struct
motors_cb_s motors_cb;

// deifnition of motor_driver from motor_driver.c
extern void (*motor_driver)(motors_t*);

// helpfull variable
const float HALF_WHEELBASE					= (WHEELBASE*0.5f);
const float TICKS_PER_MM						= (TICKS_PER_REVOLUTION/(PI*WHEEL_DIAMETER));
const float MOTOR_DRIVER_T		    	= (1.0f/MOTOR_DRIVER_FREQ);



void motor_reset()
{
	motor_port_init();
	motor_port_setPwmL(0);
	motor_port_setPwmR(0);
	memset(&motors.mot[0], 0, sizeof(motor_t));
	memset(&motors.mot[1], 0, sizeof(motor_t));
	motors.mot[0].lastEnc = motor_port_getEncL();
	motors.mot[1].lastEnc = motor_port_getEncR();
	motors.flags = MOTOR_FLAG_PID_EN|MOTOR_FLAG_ENCODER_EN;
}


void motor_init()
{
	MOTOR_ASSERT(MOTOR_MAX_PWM >= MOTOR_MAX_PWM_IN_USE);
	MOTOR_ASSERT(MOTOR_MAX_PWM_IN_USE > 0);
	MOTOR_ASSERT(motor_driver != 0);
	motor_reset();
}


static void _motor_updateEnc()
{
	// te zmienne powinny byz liczbami calkowitymi ze znakiem
	// oraz miec taka sama wielkosc jak rejestr przechowujacy
	// impulsy z eknkodera - patrz przejscia CNT przez 0!
	encInt delta;
	encInt el = motor_port_getEncL();
	encInt er = motor_port_getEncR();

	delta = el - motors.mot[0].lastEnc;
	motors.mot[0].encChange = delta;
	motors.mot[0].enc += delta;
	motors.mot[0].lastEnc = el;

	delta = er - motors.mot[1].lastEnc;
	motors.mot[1].encChange = delta;
	motors.mot[1].enc += delta;
	motors.mot[1].lastEnc = er;
}


static void _motor_updateAV(motor_t* mot)
{
	MOTOR_ASSERT(mot != 0);

	/// update accelerate and velocity
	/// increase/decrease 'temp' by 'by' to get closer to 'to'
	if(mot->vel < mot->targetVel) // accelerate
	{
		mot->a = MOTOR_ACC_V;
		mot->vel += mot->a * MOTOR_DRIVER_T;
		if(mot->vel > mot->targetVel)
			mot->vel = mot->targetVel;
	}
	else if(mot->vel > mot->targetVel) // deccelerate
	{
		mot->a = -MOTOR_ACC_V;
		mot->vel += mot->a*MOTOR_DRIVER_T;
		if(mot->vel < mot->targetVel)
			mot->vel = mot->targetVel;
	}
	else{
		mot->a = 0;
	}
}


static int _motor_roundf(float x)
{
	// zaszumianie
	static float szum = 0;
	szum += 0.013f;
	if(szum > 0.5f)
		szum -= 1.0f;
	x += szum;

	// zaokraglanie
	if(x < 0)
		return (int)(x-0.5f);
	else
		return (int)(x+0.5f);
}


static void _motor_updateVariable()
{
	// velocity
	_motor_updateAV(&motors.mot[0]);
	_motor_updateAV(&motors.mot[1]);

	// timing
	if(motors.time > 1) { // decrement translation velocity timer
		--motors.time;
	}
	else if(motors.time == 1)
	{
		--motors.time;
		motors.mot[0].targetVel = 0;
		motors.mot[1].targetVel = 0;

		// move end callback
		if(motors_cb.moveEnd != 0) {
			motors_cb.moveEnd(&motors);
		}
	}

	// position
	motors.mot[0].idealEnc += _motor_roundf(motors.mot[0].vel*MOTOR_DRIVER_T*TICKS_PER_MM);
	motors.mot[1].idealEnc += _motor_roundf(motors.mot[1].vel*MOTOR_DRIVER_T*TICKS_PER_MM);
}


static void _motor_setPWM()
{
	int VL = contrain(motors.mot[0].PWM, -MOTOR_MAX_PWM_IN_USE, MOTOR_MAX_PWM_IN_USE);
	int VR = contrain(motors.mot[1].PWM, -MOTOR_MAX_PWM_IN_USE, MOTOR_MAX_PWM_IN_USE);

	motor_port_setPwmL(VL);
	motor_port_setPwmR(VR);
}

void motor_update()
{
	if((motors.flags & MOTOR_FLAG_FREEZE) != 0)
		return;
	_motor_updateEnc();
	_motor_updateVariable();
	motor_driver(&motors);
	_motor_setPWM();
}


/// calculate distance from given last velocity[mm/s], target velocity[mm/s]
/// and time of movement[s]
static float _motor_calcS(float lastV, float vel, float t)
{
	 float a = MOTOR_ACC_V;
	 float Tacc = fabs((vel-lastV) / a);
   float Sacc = (lastV+vel)*0.5f*Tacc;
   float Scon = vel*(t-Tacc);

   if(t > Tacc)
			return Sacc+Scon;
    else
			return Sacc;
}


/// calculate time needed for moving distance s[mm] with target velocity vel[mm/s]
/// and current velocity lastV[mm/s]
static float _motor_calcTime(float lastV, float vel, float s)
{
	if(fabs(vel) < 0.001)
		return 0;
	// returns time needed for acceleration+movement with constat velocity
	float a = MOTOR_ACC_V;
	float Tacc = fabs(vel-lastV) / a;
	float Sacc = (lastV+vel)*0.5f*Tacc;
	float Tcon = fabs((s-Sacc)/vel); // czas ruchu jednostajnego
	float T;
	// gdy tylko przyspieszamy, to czas jest opisany zaleznoscia
	// t^2 + t*2*v0/a - s*s/a = 0
	if(fabs(Sacc) > fabs(s)){
		s = fabs(s);
		T = (-lastV + sqrtf(lastV*lastV+2*s*a)) / a;
		if(T < 0)
			T = (lastV + sqrtf(lastV*lastV+2*s*a)) / a;
	//gdy przyspieszamy oraz poruszamy sie ze stala predkoscia
	//T = Tacc + Tconst
	}
	else
		T = Tacc + Tcon;

	return T;
}


/// calculate target velocity[mm/s] to make distance s[mm] in time t[s]
static float _motor_calcVel(float lastV, float s, float t)
{
	// sprawdzamy, czy mamy przyspieszac, czy zwalniac
	float a = (lastV*t)>s ? -MOTOR_ACC_V : MOTOR_ACC_V;
	float V, p;
	float S = 0;

	p = (a*t*t + 2*lastV*t -2*s) / a;
	if(p < 0){
		p = (a*t*t + 2*lastV*t + 2*s)/a;
    p = sqrtf(p);
	}
	if(p >= 0)
	{
		p = sqrtf(p);
		V = lastV - (a*p) + (a*t);
		S = _motor_calcS(lastV, V, t);
		if(fabs(S-s) > 5) // gdy wyilczona predkosc jest zla
			V = lastV + a*(p+t); // to probujemy 2 rozwiazanie
	}
	else
	{ // obliczenia nie ida za dobrze
		// w akcie rozpaczy upraszczamy sobie rownania
		// doswiadczalnie zostalo dobrane to rownanie
		// inne opcje to: V = s/t (gdy t!=0), lub cos zupelnie innego
		MOTOR_LOG_WARNING("motor err calc vel (lastV:%d s:%d ms:%d)", (int)lastV, (int)s, (int)(t*1e3));
		V = lastV + a*t;
	}
	return V;
}


void motor_GoA(int left, int right, float vel)
{
	vel = fabs(vel);
	float lVl = motors.mot[0].vel; //last vel left
	float lVr = motors.mot[1].vel;
	float Vl = vel;
	float Vr = vel;
	float T;

	if(left < 0)
			Vl = -vel;
	if(right < 0)
			Vr = -vel;

	// oczlicz czas potrzebny na ruch kazdego z kol
	// i wybierz to ktore potrzebuje go wiecej
	// jako czas calego ruchu
	float Tl = _motor_calcTime(lVl, Vl, left);
	float Tr = _motor_calcTime(lVr, Vr, right);
	if(Tl > Tr) // takie same jesli jedziemy prosto
			T = Tl;
	else
			T = Tr;

	// gdy juz znasz czas, to dopasuj predkosci kazdego z kol
	// i wpisz rezultaty do struktury, aby zaczely byc obslugiwane
	// przez motor_Driver()
	Vl = _motor_calcVel(lVl, left,  T);
	Vr = _motor_calcVel(lVr, right, T);
	motor_flagSet(MOTOR_FLAG_FREEZE);
	motors.mot[0].targetVel = Vl;
	motors.mot[1].targetVel = Vr;
	motors.time = T*MOTOR_DRIVER_FREQ;
	motor_flagClear(MOTOR_FLAG_FREEZE);
}


void motor_turnA(int angle, int r, float vel)
{
	int left, right;

	motor_flagSet(MOTOR_FLAG_FREEZE);
	if(motors_cb.turnStart != 0){
		motors_cb.turnStart(&motors, &angle, &r, &vel);
	}
	motor_flagClear(MOTOR_FLAG_FREEZE);

	if(angle < 0 && r > 0) // left forward
	{
		left  = -PI*(r-HALF_WHEELBASE)*angle/180;
		right = -PI*(r+HALF_WHEELBASE)*angle/180;
	}
	else if( angle > 0 && r > 0) // right forward
	{
		left  = PI*(r+HALF_WHEELBASE)*angle/180;
		right = PI*(r-HALF_WHEELBASE)*angle/180;
	}
	else if( angle < 0 && r < 0) // left backward
	{
		left  = -PI*(r+HALF_WHEELBASE)*angle/180;
		right = -PI*(r-HALF_WHEELBASE)*angle/180;
	}
	else // right backward
	{
		left  = PI*(r-HALF_WHEELBASE)*angle/180;
		right = PI*(r+HALF_WHEELBASE)*angle/180;
	}

	motor_goA(left, right, vel);
}


void motor_setVel(int velocity, int omega)
{
	motors.time = 0;
	motors.mot[0].targetVel = velocity + omega*180.f/PI*HALF_WHEELBASE;
	motors.mot[1].targetVel = velocity - omega*180.f/PI*HALF_WHEELBASE;
}


bool motor_isReady(){
	return motors.time == 0;
}


void motor_stop(){
	motors.mot[0].targetVel = motors.mot[1].targetVel = 0;
}


void motor_go(int left, int right, float vel)
{
	motor_goA(left, right, vel);
	while(!motor_isReady()) {
	}
	motor_stop();
}


void motor_turn(int angle, int r, float vel)
{
	motor_turnA(angle, r, vel);
	while(!motor_isReady()) {
	}
	motor_stop();
}


void motor_flagSet(int flag)
{
	motors.flags |= flag;
}


void motor_flagClear(int flag)
{
	motors.flags &= ~flag;
}
