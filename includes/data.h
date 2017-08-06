struct data_st {
	char *facility;
        void (*function)(connection *u,unsigned long oid);
	int public;
	struct data_st *next;
};

extern struct data_st *datatree;
extern struct data_st *register_data(char *,void *,int);
extern void unregister_data(struct data_st *);
extern void execute_data(connection *, unsigned long, char *, int);

#define DATA_PRIVATE 0
#define DATA_PUBLIC 1
