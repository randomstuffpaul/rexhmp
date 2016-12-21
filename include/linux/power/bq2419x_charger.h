#ifndef __BQ2419X_H__
#define __BQ2419X_H__

#define INPUT_SRC_CONT		0
#define POWER_ON_CONT		1
#define CHARGE_CONT			2
#define CHARGE_TERM_CONT	3
#define CHARGE_VOLT_CONT	4
#define CHARGE_TIME_CONT	5
#define MISC_CONT			7
#define SYSTEM_STAT			8
#define FAULT_STAT			9
#define CHIP_ID				10

struct bq2419x_info {
	struct i2c_client   *client;
};

enum chip_id{
	BQ24190 = 0,
	BQ24192,
	BQ24193,
	BQ24192I,
};
enum charge_config {
	CHARGE_DISABLE = 0,
	CHARGE_BATT,
	CHARGE_OTG,
};

extern int bq2419x_get_online(struct bq2419x_info *di);
extern int bq2419x_batt_stat(struct bq2419x_info *di);
extern int bq2419x_charger_stat(struct bq2419x_info *di);
#endif
