#ifndef __GREETING_H__
#define __GREETING_H__

/**
 * Say hello to a person
 * @param name The person's name to greet
 * @return Greeting message string (static buffer)
 */
const char* say_hello(const char* name);

/**
 * Say goodbye to a person
 * @param name The person's name to say goodbye to
 * @return Farewell message string (static buffer)
 */
const char* say_goodbye(const char* name);

#endif /* __GREETING_H__ */