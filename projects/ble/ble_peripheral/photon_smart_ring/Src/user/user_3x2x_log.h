#ifndef USER_3X2X_LOG_H
#define USER_3X2X_LOG_H

typedef struct log_hr_t{
    int hr;
    int confidence;
} log_hr_t;

typedef struct log_hrv_t{
    int rri_0;
    int rri_1;
    int rri_2;
    int rri_3;
    int confidence;
    int num;
} log_hrv_t;

typedef struct log_spo2_t{
    int spo2;
    int confidence;
} log_spo2_t;

typedef struct log_adt_t{
    int adt;
    int confidence;
} log_adt_t;

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void user_3x2x_log_init(void);

int user_3x2x_log_cache_hr(log_hr_t* p_hr);
int user_3x2x_log_cache_hrv(log_hrv_t* p_hrv);
int user_3x2x_log_cache_spo2(log_spo2_t* p_spo2);
int user_3x2x_log_cache_adt(log_adt_t* p_adt);

int user_3x2x_log_print_hr(void);
int user_3x2x_log_print_hrv(void);
int user_3x2x_log_print_spo2(void);
int user_3x2x_log_print_adt(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif