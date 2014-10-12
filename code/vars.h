#define VCC_SCALE_FACTOR 60.0

// Datatypes
struct temp_package {
	uint8_t type;
	uint8_t vcc;
	uint8_t vcc_factor;
	uint16_t tempreture;
	uint8_t data[11];
};

union {
	struct temp_package info;
	uint8_t raw[16];
} typedef temp_package_t;


// Encryption stuff
uint8_t key[]  = { 0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF,
                   0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF };
