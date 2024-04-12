#pragma once
#define SINGLETON(type) public:\
							static type* GetInstance()\
								{\
									static type manager;\
									return &manager;\
								}\
						private:\
							type();\
							~type();

#define SINGLETON_PROTECTED(type) public:\
							static type* GetInstance()\
								{\
									static type manager;\
									return &manager;\
								}\
						protected:\
							type();\
							~type();