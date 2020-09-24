/* Function prototypes. */

/* main.c */
int main(int argc, char **argv);

/* store.c */
int do_publish(message *m_ptr);
int do_retrieve(message *m_ptr);
int do_subscribe(message *m_ptr);
int do_getsysinfo(message *m_ptr);
