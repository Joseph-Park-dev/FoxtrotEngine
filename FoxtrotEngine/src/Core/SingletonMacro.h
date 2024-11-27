#pragma once
// For making singleton classes.
// Don't forget to call Destory() to delete mObject!
#define SINGLETON(TYPE) public:\
							static TYPE* GetInstance() \
								{ \
									if(mInstance == nullptr) \
										mInstance = new TYPE(); \
									return mInstance; \
								} \
							static void Destroy() \
								{ \
									if (mInstance) \
									{ \
										delete mInstance; \
										mInstance = nullptr; \
									} \
								} \
							TYPE(const TYPE& obj) = delete; \
						private:\
							TYPE(); \
							~TYPE(); \
							static TYPE* mInstance;

// For making singleton classes which allows to be inherited.
// Don't forget to call Destory() to delete mObject!
#define SINGLETON_PROTECTED(TYPE) public:\
							static TYPE* GetInstance() \
								{ \
									if(mInstance == nullptr) \
										mInstance = new TYPE(); \
									return mInstance; \
								} \
							static void Destroy() \
								{ \
									delete mInstance; \
								} \
							TYPE(const TYPE& obj) = delete; \
						protected:\
							TYPE(); \
							~TYPE(); \
							static TYPE* mInstance;