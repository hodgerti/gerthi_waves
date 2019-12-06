#ifndef DELTA_TIME_H
#define DELTA_TIME_H

/************************************
* Headers
*************************************/

/************************************
* Constants
*************************************/

/************************************
* Types
*************************************/
typedef double (*get_time_callback_func)(void);

class DeltaTime
{
	public:
		get_time_callback_func time_func;
		double last_time = 0.0f;
		double current_time = 0.0f;
		double delta = 0.0f;

		DeltaTime()
		{

		}

		DeltaTime(get_time_callback_func func_p)
		{
			time_func = func_p;
		}

		// set time function callback
		void set_get_time_function(get_time_callback_func func_p)
		{
			time_func = func_p;
		}

		void update()
		{
			current_time = time_func();
			delta = current_time - last_time;
			last_time = current_time;
		}

		double operator =(float x)
		{
			return delta;
		}

};


/************************************
* Functions
*************************************/

#endif