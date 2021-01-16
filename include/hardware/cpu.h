#include "common/types.h"

#define CPU_FEATURE_SSE3 			0
#define CPU_FEATURE_PCLMULQDQ 		1
#define CPU_FEATURE_DTES64 			2
#define CPU_FEATURE_MONITOR 		3
#define CPU_FEATURE_DS_CPL 			4
#define CPU_FEATURE_VMX 			5
#define CPU_FEATURE_SMX 			6
#define CPU_FEATURE_EST 			7
#define CPU_FEATURE_TM2 			8
#define CPU_FEATURE_SSSE3 			9
#define CPU_FEATURE_CNXT_ID 		10
#define CPU_FEATURE_SDBG 			11
#define CPU_FEATURE_FMA 			12
#define CPU_FEATURE_CX16 			13
#define CPU_FEATURE_CPTR 			14
#define CPU_FEATURE_PDCM 			15
// reserved
#define CPU_FEATURE_PCID 			17
#define CPU_FEATURE_DCA 			18
#define CPU_FEATURE_SSE4_1 			19
#define CPU_FEATURE_SSE4_2 			20
#define CPU_FEATURE_X2APIC 			21
#define CPU_FEATURE_MOVBE 			22
#define CPU_FEATURE_POPCNT 			23
#define CPU_FEATURE_TSC_DEADLINE 	24
#define CPU_FEATURE_AES 			25
#define CPU_FEATURE_XSAVE 			26
#define CPU_FEATURE_OSXSAVE 		27
#define CPU_FEATURE_AVX 			28
#define CPU_FEATURE_F16C 			29
#define CPU_FEATURE_RDRND 			30
#define CPU_FEATURE_HYPERVISOR 		31


#define CPU_FEATURE_FPU 			32
#define CPU_FEATURE_VME 			33
#define CPU_FEATURE_DE 				34
#define CPU_FEATURE_PSE 			35
#define CPU_FEATURE_TSC 			36
#define CPU_FEATURE_MSR 			37
#define CPU_FEATURE_PAE 			38
#define CPU_FEATURE_MCE 			39
#define CPU_FEATURE_CX8 			40
#define CPU_FEATURE_APIC 			41
// reserved
#define CPU_FEATURE_SEP 			43
#define CPU_FEATURE_MTRR 			44
#define CPU_FEATURE_PGE 			45
#define CPU_FEATURE_MCA 			46
#define CPU_FEATURE_CMOV 			47
#define CPU_FEATURE_PAT 			48
#define CPU_FEATURE_PSE_36 			49
#define CPU_FEATURE_PSN 			50
#define CPU_FEATURE_CLFSH 			51
// reserved
#define CPU_FEATURE_DS 				53
#define CPU_FEATURE_ACPI 			54
#define CPU_FEATURE_MMX 			55
#define CPU_FEATURE_FXSR 			56
#define CPU_FEATURE_SSE 			57
#define CPU_FEATURE_SSE2 			58
#define CPU_FEATURE_SS 				59
#define CPU_FEATURE_HTT 			60
#define CPU_FEATURE_TM 				61
#define CPU_FEATURE_IA64 			62
#define CPU_FEATURE_PBE 			63

class CPU {

    private:
        static CPU * activeCPU;

        uint64_t cpuid;

    public:
        CPU();
        ~CPU();

        void DetectFeatures();

        bool EnableSSE();

        inline bool HasFeature(uint8_t feature) {
            return cpuid & (static_cast<uint64_t>(1) << feature);
        }

        static inline bool CPUHasFeature(uint8_t feature) {
            if(activeCPU != nullptr)
                return activeCPU->HasFeature(feature);
            return false;
        }

};