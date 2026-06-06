// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the GENERICDATA_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// GENERICDATA_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GENERICDATA_EXPORTS
#define GENERICDATA_API __declspec(dllexport)
#else
#define GENERICDATA_API __declspec(dllimport)
#endif

// This class is exported from the dll
class GENERICDATA_API CGenericData {
public:
	CGenericData(void);
	// TODO: add your methods here.
};

extern GENERICDATA_API int nGenericData;

GENERICDATA_API int fnGenericData(void);
