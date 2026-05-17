#include "common/types.h"

u32 I2c_BuckConverter_MultiplierToUvOut(const I2c_BuckConverter_Domain* domain, u8 multiplier) {
    // Function implementation
}

u8 I2c_BuckConverter_MvOutToMultiplier(const I2c_BuckConverter_Domain* domain, u32 mvolt) {
    // Function implementation
}

u32 I2c_BuckConverter_GetMvOut(const I2c_BuckConverter_Domain* domain) {
    // Function implementation
}

u32 I2c_BuckConverter_GetUvOut(const I2c_BuckConverter_Domain* domain) {
    // Function implementation
}

Result I2c_BuckConverter_SetMvOut(const I2c_BuckConverter_Domain* domain, u32 mvolt) {
    // Function implementation
}

u8 I2c_Bq24193_Convert_mA_Raw(u32 ma) {
    // Function implementation
}

u32 I2c_Bq24193_Convert_Raw_mA(u8 raw) {
    // Function implementation
}

Result I2c_Bq24193_GetFastChargeCurrentLimit(u32 *ma) {
    // Function implementation
}

Result I2c_Bq24193_SetFastChargeCurrentLimit(u32 ma) {
    // Function implementation
}