#include <stdio.h>   /* standard I/O routines                 */
#include <pthread.h> /* pthread functions and data structures */
#include <stdbool.h>
#include <semaphore.h>
/* function to be executed by the new thread */
#define REGISTRATION_SIZE 10
#define RESTROOM_SIZE 10
#define CAFE_NUMBER 10
#define GP_NUMBER 10
#define PHARMACY_NUMBER 10
#define BLOOD_LAB_NUMBER 10
#define OR_NUMBER 10
#define SURGEON_NUMBER 30
#define NURSE_NUMBER 30
#define SURGEON_LIMIT 5
#define NURSE_LIMIT 5
#define PATIENT_NUMBER 1000 //1000
int HOSPITAL_WALLET = 0;

#define WAIT_TIME 100
#define REGISTRATION_TIME 100
#define GP_TIME 200
#define PHARMACY_TIME 100
#define BLOOD_LAB_TIME 200
#define SURGERY_TIME 500
#define CAFE_TIME 100
#define RESTROOM_TIME 100

#define Hunger_meter 100
#define Restroom_meter 100
int HUNGER_INCREASE_RATE = 10;
int RESTROOM_INCREASE_RATE = 10;
int hunger_Restroom_limit = 1000;

int REGISTRATION_COST = 100;
int PHARMACY_COST = 200; // Calculated randomly between 1 and given value.
int BLOOD_LAB_COST = 200;
int SURGERY_OR_COST = 200;
int SURGERY_SURGEON_COST = 100;
int SURGERY_NURSE_COST = 50;
int CAFE_COST = 200; // Calculated randomly between 1 and given value.

int registrations_info[REGISTRATION_SIZE];
int patient_info[PATIENT_NUMBER][5];
int GP_info[GP_NUMBER];
int Surgeon_info[SURGEON_NUMBER];
int Nurse_info[NURSE_NUMBER];
int OR_info[OR_NUMBER][10];
int blood_info[BLOOD_LAB_NUMBER];
int pharmacy_info[PHARMACY_NUMBER];

pthread_mutex_t Register_mutex;
pthread_mutex_t Patient_mutex;
pthread_mutex_t Patient_GP_mutex;
pthread_mutex_t Patient_blood_mutex;
pthread_mutex_t Patient_pharmacy_mutex;
pthread_mutex_t GP_mutex;
pthread_mutex_t blood_mutex;
pthread_mutex_t pharmacy_mutex;
pthread_mutex_t OR_mutex;
sem_t Surgeon_semaphore;
sem_t Nurse_semaphore;
pthread_mutex_t Cleaning;
pthread_mutex_t Surgery_mutex;
pthread_mutex_t selecting_n;
pthread_mutex_t selecting_s;
sem_t cafe;
sem_t rest;
pthread_mutex_t money;

bool is_reg = false;
bool is_GP = false;
bool is_blood = false;
bool is_pharmacy = false;
bool is_OR = false;
bool is_nurse = false;
bool is_surgeon = false;

int Reg_temp = 0; //hasta reg eşleştirmesi için
int GP_temp = 0;
int blood_temp = 0;
int pharmacy_temp = 0;
int OR_temp = 0;
int nurse_temp = 0;
int surgeon_temp = 0;
void *wait(int patients_id)
{
    pthread_mutex_lock(&Patient_mutex);
    patient_info[patients_id - 1][4] = rand() % WAIT_TIME;
    pthread_mutex_unlock(&Patient_mutex);
    usleep(patient_info[patients_id - 1][4]);
    patient_info[patients_id - 1][2] += HUNGER_INCREASE_RATE * patient_info[patients_id - 1][4];
    patient_info[patients_id - 1][3] += RESTROOM_INCREASE_RATE * patient_info[patients_id - 1][4];
    
    switch (patient_info[patients_id - 1][2] )
    {
        case 1000 ... 99999:
        {
                patient_info[patients_id - 1][2] = 0;
                sem_wait(&cafe);
                printf("patient %d in cafe\n", patients_id);//hasta kafeye giris yapiyor
                usleep(rand() % CAFE_TIME);
                printf("patient %d out cafe\n", patients_id);
                pthread_mutex_lock(&money);
                HOSPITAL_WALLET += rand() % CAFE_COST;
                pthread_mutex_unlock(&money);
                sem_post(&cafe);
        }
       
    }
    switch (patient_info[patients_id - 1][3])
    {
        case 1000 ... 99999: ////hastanin  dinlenme odasi islemleri
        {
            patient_info[patients_id - 1][3] = 0;
            sem_wait(&rest);
            printf("patient %d in Restroom\n", patients_id);
            usleep(rand() % RESTROOM_TIME);
            printf("patient %d out Restroom\n", patients_id);
            sem_post(&rest);
        }
       
    }
}
void *Patient(int patients_id)
{
    wait(patients_id);

    pthread_mutex_lock(&Patient_mutex);
    while (!is_reg)
    {
    }
    patient_info[patients_id - 1][0] = Reg_temp;
    printf("patient (%d) has been enter Registration Desk(%d) \n", patients_id, patient_info[patients_id - 1][0]);
    is_reg = false;
    pthread_mutex_unlock(&Register_mutex);
    pthread_mutex_unlock(&Patient_mutex);
    usleep(rand() % REGISTRATION_TIME);
    registrations_info[patient_info[patients_id - 1][0]] = 0; //registeri serbest bırakıyor.
    printf("patient (%d) has been exit Registration Desk(%d) \n", patients_id, patient_info[patients_id - 1][0]);
    pthread_mutex_lock(&money);
    HOSPITAL_WALLET += REGISTRATION_COST;
    pthread_mutex_unlock(&money);

    wait(patients_id);
    pthread_mutex_lock(&Patient_GP_mutex);
    while (!is_GP)
    {
    }
    patient_info[patients_id - 1][0] = GP_temp;
    printf("patient (%d) has been enter GP (%d) \n", patients_id, patient_info[patients_id - 1][0]);
    is_GP = false;
    pthread_mutex_unlock(&GP_mutex);
    pthread_mutex_unlock(&Patient_GP_mutex);
    usleep(rand() % GP_TIME);
    GP_info[patient_info[patients_id - 1][0]] = 0; //GPyi serbest bırakıyor.
    printf("patient (%d) has ben exit GP (%d) \n", patients_id, patient_info[patients_id - 1][0]);
    switch (patient_info[patients_id][1])
    {
        case 0:
        {
            wait(patients_id);
        pthread_mutex_lock(&Patient_blood_mutex);
        while (!is_blood)
        {
        }
        patient_info[patients_id - 1][0] = blood_temp;
        printf("patient (%d) has been enter blood lab (%d) \n", patients_id, patient_info[patients_id - 1][0]);
        is_blood = false;
        pthread_mutex_unlock(&blood_mutex);
        pthread_mutex_unlock(&Patient_blood_mutex);
        usleep(rand() % BLOOD_LAB_TIME);
        blood_info[patient_info[patients_id - 1][0]] = 0; //blood labı serbest bırakıyor.
        printf("patient (%d) has been exit blood lab (%d) \n", patients_id, patient_info[patients_id - 1][0]);

        pthread_mutex_lock(&money);
        HOSPITAL_WALLET += BLOOD_LAB_COST;
        pthread_mutex_unlock(&money);

        wait(patients_id);
        pthread_mutex_lock(&Patient_GP_mutex);
        while (!is_GP)
        {
        }
        patient_info[patients_id - 1][0] = GP_temp;
        printf("patient (%d) return GP (%d) \n", patients_id, patient_info[patients_id - 1][0]);
        is_GP = false;
        pthread_mutex_unlock(&GP_mutex);
        pthread_mutex_unlock(&Patient_GP_mutex);
        usleep(rand() % GP_TIME);
        GP_info[patient_info[patients_id - 1][0]] = 0; //GPyi serbest bırakıyor.
        printf("patient (%d) has been exit GP and Hospital (%d) \n", patients_id, patient_info[patients_id - 1][0]);

        }
        case 1:
        {
            wait(patients_id);
        pthread_mutex_lock(&Patient_pharmacy_mutex);
        while (!is_pharmacy)
        {
        }
        patient_info[patients_id - 1][0] = pharmacy_temp;
        printf("patient (%d) has been enter Pharmacy (%d) \n", patients_id, patient_info[patients_id - 1][0]);
        is_pharmacy = false;
        pthread_mutex_unlock(&pharmacy_mutex);
        pthread_mutex_unlock(&Patient_pharmacy_mutex);
        usleep(rand() % PHARMACY_TIME);
        pharmacy_info[patient_info[patients_id - 1][0]] = 0; //pharmacyi serbest bırakıyor.
        printf("patient (%d) has been exit Pharmacy (%d) and Hospital \n", patients_id, patient_info[patients_id - 1][0]);

        pthread_mutex_lock(&money);
        HOSPITAL_WALLET += rand() % PHARMACY_COST;
        pthread_mutex_unlock(&money);
        }
        case 2:
        {
            wait(patients_id);
        pthread_mutex_lock(&Patient_blood_mutex);
        while (!is_blood)
        {
        }
        patient_info[patients_id - 1][0] = blood_temp;
        printf("patient (%d) has been enter blood lab (%d) \n", patients_id, patient_info[patients_id - 1][0]);
        is_blood = false;
        pthread_mutex_unlock(&blood_mutex);
        pthread_mutex_unlock(&Patient_blood_mutex);
        usleep(rand() % BLOOD_LAB_TIME);
        blood_info[patient_info[patients_id - 1][0]] = 0; //blood labı serbest bırakıyor.
        printf("patient (%d) has been exit blood lab (%d) \n", patients_id, patient_info[patients_id - 1][0]);

        pthread_mutex_lock(&money);
        HOSPITAL_WALLET += BLOOD_LAB_COST;
        pthread_mutex_unlock(&money);

        wait(patients_id);
        pthread_mutex_lock(&Patient_GP_mutex);
        while (!is_GP)
        {
        }
        patient_info[patients_id - 1][0] = GP_temp;
        printf("patient (%d) return GP (%d) \n", patients_id, patient_info[patients_id - 1][0]);
        is_GP = false;
        pthread_mutex_unlock(&GP_mutex);
        pthread_mutex_unlock(&Patient_GP_mutex);
        usleep(rand() % GP_TIME);
        GP_info[patient_info[patients_id - 1][0]] = 0; //GPyi serbest bırakıyor.
        printf("patient (%d) again has been exit GP (%d) and send Pharmacy\n", patients_id, patient_info[patients_id - 1][0]);

        wait(patients_id);
        pthread_mutex_lock(&Patient_pharmacy_mutex);
        while (!is_pharmacy)
        {
        }
        patient_info[patients_id - 1][0] = pharmacy_temp;
        printf("patient (%d) in Pharmacy (%d) \n", patients_id, patient_info[patients_id - 1][0]);
        is_pharmacy = false;
        pthread_mutex_unlock(&pharmacy_mutex);
        pthread_mutex_unlock(&Patient_pharmacy_mutex);
        usleep(rand() % PHARMACY_TIME);
        pharmacy_info[patient_info[patients_id - 1][0]] = 0; //pharmacyi serbest bırakıyor.
        printf("patient (%d) has been exit Pharmacy (%d) and Hospital \n", patients_id, patient_info[patients_id - 1][0]);

        pthread_mutex_lock(&money);
        HOSPITAL_WALLET += rand() % PHARMACY_COST;
        pthread_mutex_unlock(&money);

        }
        case 3:
        {
             wait(patients_id);
        pthread_mutex_lock(&Surgery_mutex);
        while (!is_OR)
        {
        }
        patient_info[patients_id - 1][0] = OR_temp;
        printf("patient (%d) has been enter OR (%d) \n", patients_id, patient_info[patients_id - 1][0]);

        int nurse_req = rand() % (SURGEON_LIMIT - 1) + 1;
        int surgeon_req = rand() % (NURSE_LIMIT - 1) + 1;
        pthread_mutex_lock(&money);
        HOSPITAL_WALLET += SURGERY_OR_COST + (surgeon_req * SURGERY_SURGEON_COST) + (nurse_req * SURGERY_NURSE_COST);
        pthread_mutex_unlock(&money);
        printf("nurse %d surgeon %d\n", nurse_req, surgeon_req);
        int u=0;
        int m=0;
        while (u< nurse_req)
        {
            u++;
            while (!is_nurse)
            {
            }
            OR_info[OR_temp - 1][u + 6] = nurse_temp;
            is_nurse = false;
            sem_post(&Nurse_semaphore);
            pthread_mutex_lock(&selecting_n);
        }
        while (m< surgeon_req)
        {
            m++;
            while (!is_surgeon)
            {
            }
            OR_info[OR_temp - 1][m + 1] = surgeon_temp;
            is_surgeon = false;

            sem_post(&Surgeon_semaphore);
            pthread_mutex_lock(&selecting_s);
        }
        is_OR = false;
        printf("(%d) nurse  (%d) surgeon OR (%d) operation has begun\n", nurse_req, surgeon_req, OR_temp);
        pthread_mutex_unlock(&OR_mutex);
        pthread_mutex_unlock(&Surgery_mutex);

        usleep(rand() % 500);

        pthread_mutex_lock(&Cleaning);
        int j=0;

        while(j < 11)
        {
            j++;
            if (OR_info[patient_info[patients_id - 1][0] - 1][j] != 0)
            {
                switch(j)
                {
                    case 0 ... 6:
                    Surgeon_info[OR_info[patient_info[patients_id - 1][0] - 1][j] - 1] = 0;
                    default:
                    Nurse_info[OR_info[patient_info[patients_id - 1][0] - 1][j] - 1] = 0;

                }
               
                OR_info[patient_info[patients_id - 1][0] - 1][j] = 0;
            }
        }
        OR_info[patient_info[patients_id - 1][0] - 1][0] = 0;
        printf("patient (%d) left OR (%d) operation has finished.\n", patients_id, patient_info[patients_id - 1][0]);
        pthread_mutex_unlock(&Cleaning);

        wait(patients_id);
        pthread_mutex_lock(&Patient_GP_mutex);
        while (!is_GP)
        {
        }
        patient_info[patients_id - 1][0] = GP_temp;
        printf("patient (%d) return GP (%d) \n", patients_id, patient_info[patients_id - 1][0]);
        is_GP = false;
        pthread_mutex_unlock(&GP_mutex);
        pthread_mutex_unlock(&Patient_GP_mutex);
        usleep(rand() % GP_TIME);
        GP_info[patient_info[patients_id - 1][0]] = 0; //GPyi serbest bırakıyor.
        printf("patient (%d) again has been exit GP (%d) and send Pharmacy\n", patients_id, patient_info[patients_id - 1][0]);

        wait(patients_id);
        pthread_mutex_lock(&Patient_pharmacy_mutex);
        while (!is_pharmacy)
        {
        }
        patient_info[patients_id - 1][0] = pharmacy_temp;
        printf("patient (%d) has been entered Pharmacy (%d) \n", patients_id, patient_info[patients_id - 1][0]);
        is_pharmacy = false;
        pthread_mutex_unlock(&pharmacy_mutex);
        pthread_mutex_unlock(&Patient_pharmacy_mutex);
        usleep(rand() % PHARMACY_TIME);
        pharmacy_info[patient_info[patients_id - 1][0]] = 0; //pharmacyi serbest bırakıyor.
        printf("patient (%d) has been exited Pharmacy and Hospital(%d) \n", patients_id, patient_info[patients_id - 1][0]);
        pthread_mutex_lock(&money);
        HOSPITAL_WALLET += rand() % PHARMACY_COST;
        pthread_mutex_unlock(&money);

        }
        case 4 ... 99:
        {
             wait(patients_id);
        pthread_mutex_lock(&Surgery_mutex);
        while (!is_OR)
        {
        }
        patient_info[patients_id - 1][0] = OR_temp;
        printf("patient (%d) has been entered OR (%d) \n", patients_id, patient_info[patients_id - 1][0]);

        int nurse_req = rand() % (SURGEON_LIMIT - 1) + 1;
        int surgeon_req = rand() % (NURSE_LIMIT - 1) + 1;
        pthread_mutex_lock(&money);
        HOSPITAL_WALLET += SURGERY_OR_COST + (surgeon_req * SURGERY_SURGEON_COST) + (nurse_req * SURGERY_NURSE_COST);
        pthread_mutex_unlock(&money);
        printf("nurse %d surgeon %d\n", nurse_req, surgeon_req);
        int z=0;
        int y=0;
        int x=0;
        while (z < nurse_req)
        { 
            z++;
            while (!is_nurse)
            {
            }
            OR_info[OR_temp - 1][z + 6] = nurse_temp;
            is_nurse = false;
            sem_post(&Nurse_semaphore);
            pthread_mutex_lock(&selecting_n);
        }
        while (y< surgeon_req)
        {
            y++;
            while (!is_surgeon)
            {
            }
            OR_info[OR_temp - 1][y + 1] = surgeon_temp;
            is_surgeon = false;

            sem_post(&Surgeon_semaphore);
            pthread_mutex_lock(&selecting_s);
        }
        is_OR = false;
        printf("(%d) nurse  (%d) surgeon OR (%d) operation has begun\n", nurse_req, surgeon_req, OR_temp);
        pthread_mutex_unlock(&OR_mutex);
        pthread_mutex_unlock(&Surgery_mutex);

        usleep(rand() % 500);

        pthread_mutex_lock(&Cleaning);

        while (x < 11)
        {
            x++;
            if (OR_info[patient_info[patients_id - 1][0] - 1][x] != 0)
            {
                switch(x)
                {
                    case 0 ... 6:
                    Surgeon_info[OR_info[patient_info[patients_id - 1][0] - 1][x] - 1] = 0;
                    default:
                    Nurse_info[OR_info[patient_info[patients_id - 1][0] - 1][x] - 1] = 0;

                }
                
                OR_info[patient_info[patients_id - 1][0] - 1][x] = 0;
            }
        }
        OR_info[patient_info[patients_id - 1][0] - 1][0] = 0;
        printf("patient (%d) left OR (%d) operation has done.\n", patients_id, patient_info[patients_id - 1][0]);
        pthread_mutex_unlock(&Cleaning);

        wait(patients_id);
        pthread_mutex_lock(&Patient_GP_mutex);
        while (!is_GP)
        {
        }
        patient_info[patients_id - 1][0] = GP_temp;
        printf("patient (%d) return GP (%d) from operation \n", patients_id, patient_info[patients_id - 1][0]);
        is_GP = false;
        pthread_mutex_unlock(&GP_mutex);
        pthread_mutex_unlock(&Patient_GP_mutex);
        usleep(rand() % GP_TIME);
        GP_info[patient_info[patients_id - 1][0]] = 0; //GPyi serbest bırakıyor.
        printf("patient (%d) again left GP (%d) and Hospital\n", patients_id, patient_info[patients_id - 1][0]);

        }

    }

     printf("Hospital wallet :%d\n",HOSPITAL_WALLET);
}
void *Registration(int register_id)
{
    registrations_info[register_id] = 0;
    int reg=0;
    
    do{

       
        reg=registrations_info[register_id];
        switch(reg){
         case 0:
        {
           pthread_mutex_lock(&Register_mutex);
            registrations_info[register_id] = 1;
            Reg_temp = register_id;
            is_reg = true;
        }
        default:
        {
         pthread_mutex_lock(&Register_mutex);
         pthread_mutex_unlock(&Register_mutex);
        }
        }
       
            
        
    }while (1 == 1);
}
void *GP(int GP_id)
{
    GP_info[GP_id] = 0;
    do
    {

        
        switch(GP_info[GP_id])
        {
        case 0:
        {
            pthread_mutex_lock(&GP_mutex);
            GP_info[GP_id] = 1;
            GP_temp = GP_id;
            is_GP = true;
        }
        default:
        {
            pthread_mutex_lock(&GP_mutex);
            pthread_mutex_unlock(&GP_mutex);
        }
        }
        
    }while(1==1);
}
void *blood(int blood_id)
{
    blood_info[blood_id] = 0;
    do
    {

       switch(blood_info[blood_id])
       {case 0:
        {
            pthread_mutex_lock(&blood_mutex);
            blood_info[blood_id] = 1;
            blood_temp = blood_id;
            is_blood = true;
        }
        default:
        {
            pthread_mutex_lock(&blood_mutex);
            pthread_mutex_unlock(&blood_mutex);
        }
        }
        
    }while (1 == 1);
}
void *pharmacy(int pharmacy_id)
{
    pharmacy_info[pharmacy_id] = 0;
    
    do{

        switch(pharmacy_info[pharmacy_id])
        {
        case 0:
        {
            pthread_mutex_lock(&pharmacy_mutex);
            pharmacy_info[pharmacy_id] = 1;
            pharmacy_temp = pharmacy_id;
            is_pharmacy = true;
        }
        default:
        {
            pthread_mutex_lock(&pharmacy_mutex);
            pthread_mutex_unlock(&pharmacy_mutex);
        }
        }
        
    }while (1 == 1);
}
void *OR(int OR_id)
{
    OR_info[OR_id - 1][0] = 0;
    
    do{

       switch(OR_info[OR_id - 1][0])
       { 
        case 0:
        {   
             pthread_mutex_lock(&OR_mutex);
            OR_info[OR_id - 1][0] = 1;
            OR_temp = OR_id;
            is_OR = true;
        }
        default:
        {
             pthread_mutex_lock(&OR_mutex);
            pthread_mutex_unlock(&OR_mutex);
        }
       }
        
    }while (1 == 1);
}
void *Surgeon(int Surgeon_id)
{
    Surgeon_info[Surgeon_id - 1] = 0;
    
    do{
         switch(Surgeon_info[Surgeon_id - 1])
         {
		 case 0:
		{
		   sem_wait(&Surgeon_semaphore);
		    Surgeon_info[Surgeon_id - 1] = 1;
		    surgeon_temp = Surgeon_id;
		    is_surgeon = true;
		    printf("surgeon %d is choosed", Surgeon_id);
		    pthread_mutex_unlock(&selecting_s);
		}
		default:
		{ 
		   sem_wait(&Surgeon_semaphore);

		    sem_post(&Surgeon_semaphore);
		}
         }

        
    }while (1 == 1);
}
void *Nurse(int Nurse_id)
{
    Nurse_info[Nurse_id - 1] = 0;
    
    do{
        switch(Nurse_info[Nurse_id - 1] )
        {
		case 0:
		{
		    sem_wait(&Nurse_semaphore);
		    Nurse_info[Nurse_id - 1] = 1;
		    nurse_temp = Nurse_id;
		    is_nurse = true;
		    printf("nurse %d is choosed", Nurse_id);
		    pthread_mutex_unlock(&selecting_n);
		}
		default:
		{
		   sem_wait(&Nurse_semaphore);

		    sem_post(&Nurse_semaphore);
		}
        }

        
    }while (1 == 1);
}
int main(int argc, char *argv[])
{

    int rc; /* return value */                         
    pthread_t Surgeon_thread;
    pthread_t Nurse_thread;
    pthread_t Blood_thread;
    pthread_t Pharmacy_thread;
    pthread_t Patient_thread;
    pthread_t Registration_thread;
    pthread_t OR_thread;
    pthread_t GP_thread;
    sem_init(&Surgeon_semaphore, 0, 1);
    sem_init(&Nurse_semaphore, 0, 1);
    sem_init(&cafe, 0, CAFE_NUMBER);
    sem_init(&rest, 0, RESTROOM_SIZE);
    int a=0;
    int b=0;
    int c=0;
    int d=0;
    int e=0;
    int f=0;
    int g=0;
    int h=0;
    
    while(a < REGISTRATION_SIZE)
    {
        a++;

        rc = pthread_create(&Registration_thread, NULL, Registration, a + 1);
        if (rc)
        {
            printf("\n ERROR: return code from pthread_create is %d \n", rc);
            exit(1);
        }
    }
    while(b < GP_NUMBER)
    {
        b++;

        rc = pthread_create(&GP_thread, NULL, GP, b + 1);
        if (rc)
        {
            printf("\n ERROR: return code from pthread_create is %d \n", rc);
            exit(1);
        }
    }
    while (c< BLOOD_LAB_NUMBER)
    {
        c++;

        rc = pthread_create(&Blood_thread, NULL, blood, c + 1);
        if (rc)
        {
            printf("\n ERROR: return code from pthread_create is %d \n", rc);
            exit(1);
        }
    }
    while(d < PHARMACY_NUMBER)
    {
        d++;

        rc = pthread_create(&Pharmacy_thread, NULL, pharmacy, d + 1);
        if (rc)
        {
            printf("\n ERROR: return code from pthread_create is %d \n", rc);
            exit(1);
        }
    }
    while (e < SURGEON_NUMBER)
    {
        e++;

        rc = pthread_create(&Surgeon_thread, NULL, Surgeon, e + 1);
        if (rc)
        {
            printf("\n ERROR: return code from pthread_create is %d \n", rc);
            exit(1);
        }
    }
    while (f< NURSE_NUMBER)
    {
        f++;

        rc = pthread_create(&Nurse_thread, NULL, Nurse, f + 1);
        if (rc)
        {
            printf("\n ERROR: return code from pthread_create is %d \n", rc);
            exit(1);
        }
    }
    while (g < OR_NUMBER)
    {
        g++;

        rc = pthread_create(&OR_thread, NULL, OR, g + 1);
        if (rc)
        {
            printf("\n ERROR: return code from pthread_create is %d \n", rc);
            exit(1);
        }
    }
    while (h < PATIENT_NUMBER)
    {
        h++;

        patient_info[h][1] = rand() % 5;
        patient_info[h][2] = rand() % Hunger_meter;
        patient_info[h][3] = rand() % Restroom_meter;
        rc = pthread_create(&Patient_thread, NULL, Patient, h + 1);
        if (rc)
        {
            printf("\n ERROR: return code from pthread_create is %d \n", rc);
            exit(1);
        }
    }

    pthread_join(Patient_thread, NULL);
    pthread_join(Nurse_thread, NULL);
    pthread_join(Blood_thread, NULL);
    pthread_join(Pharmacy_thread, NULL);
    pthread_join(Registration_thread, NULL);
    pthread_join(OR_thread, NULL);
    pthread_join(GP_thread, NULL);
   
    pthread_exit(NULL);
}
