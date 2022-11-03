#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)             \
do {                                    \
  printf("ASSERT: " #assert " ... ");   \
  if (assert) {                         \
    printf("\033[0;32mPASS\n\033[0m");  \
  } else {                              \
    printf("\033[0;31mFAIL\n\033[0m");  \
    exit(1);                            \
  }                                     \
} while(0)

/* Test Queue Create */
void test_create(void)
{
  fprintf(stderr, "\033[0;33m*** TEST create ***\033[0m\n");

  queue_t q;

  q = queue_create();
  TEST_ASSERT(q != NULL);

  TEST_ASSERT(queue_length(q) == 0);
  TEST_ASSERT(queue_destroy(q) == 0);
}

/* Test Enqueue/Dequeue simple */
void test_queue_simple(void)
{
  int data = 3, * ptr;
  queue_t q;

  fprintf(stderr, "\033[0;33m*** TEST Queue Simple ***\033[0m\n");

  q = queue_create();
  queue_enqueue(q, &data);
  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &data);

  TEST_ASSERT(queue_length(q) == 0);
  TEST_ASSERT(queue_destroy(q) == 0);
}

/* Several Enqueues */
void test_several_enqueues(void)
{
  int val1 = 1;
  int val2 = 2;
  int val3 = 3;
  int* ptr1 = &val1;
  int* ptr2 = &val2;
  int* ptr3 = &val3;

  queue_t q;
  int* ptr;

  fprintf(stderr, "\033[0;33m*** TEST Several Enqueues ***\033[0m\n");

  q = queue_create();
  TEST_ASSERT(queue_enqueue(q, ptr1) == 0);
  TEST_ASSERT(queue_enqueue(q, ptr2) == 0);
  TEST_ASSERT(queue_enqueue(q, ptr3) == 0);
  TEST_ASSERT(queue_length(q) == 3);

  TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == 0);
  TEST_ASSERT(ptr == ptr1);

  TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == 0);
  TEST_ASSERT(ptr == ptr2);

  TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == 0);
  TEST_ASSERT(ptr == ptr3);
  TEST_ASSERT(queue_length(q) == 0);
  TEST_ASSERT(queue_destroy(q) == 0);
}

/* Queue Delete Test */
void test_queue_delete(void)
{
  int val1 = 1;
  int val2 = 2;
  int val3 = 3;
  int* ptr1 = &val1;
  int* ptr2 = &val2;
  int* ptr3 = &val3;

  int* ptr;
  queue_t q;

  fprintf(stderr, "\033[0;33m*** TEST Queue Delete ***\033[0m\n");

  q = queue_create();
  queue_enqueue(q, ptr1);
  queue_enqueue(q, ptr2);
  queue_enqueue(q, ptr3);
  TEST_ASSERT(queue_length(q) == 3);

  TEST_ASSERT(queue_delete(q, ptr2) == 0);
  TEST_ASSERT(queue_length(q) == 2);

  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == ptr1);

  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == ptr3);
  TEST_ASSERT(queue_length(q) == 0);
  TEST_ASSERT(queue_destroy(q) == 0);
}
/* Queue Delete Head Test */
void test_queue_delete_head(void)
{
  int val1 = 1;
  int val2 = 2;
  int val3 = 3;
  int* ptr1 = &val1;
  int* ptr2 = &val2;
  int* ptr3 = &val3;

  queue_t q;

  fprintf(stderr, "\033[0;33m*** TEST Queue Delete Head ***\033[0m\n");

  q = queue_create();
  queue_enqueue(q, ptr1);
  queue_enqueue(q, ptr2);
  queue_enqueue(q, ptr3);
  TEST_ASSERT(queue_length(q) == 3);

  TEST_ASSERT(queue_delete(q, ptr1) == 0);
  TEST_ASSERT(queue_length(q) == 2);

  TEST_ASSERT(queue_delete(q, ptr2) == 0);
  TEST_ASSERT(queue_length(q) == 1);

  TEST_ASSERT(queue_delete(q, ptr3) == 0);
  TEST_ASSERT(queue_length(q) == 0);
  TEST_ASSERT(queue_destroy(q) == 0);
}
/* Queue Delete Tail Test */
void test_queue_delete_tail(void)
{
  int val1 = 1;
  int val2 = 2;
  int val3 = 3;
  int* ptr1 = &val1;
  int* ptr2 = &val2;
  int* ptr3 = &val3;

  queue_t q;

  fprintf(stderr, "\033[0;33m*** TEST Queue Delete Tail ***\033[0m\n");

  q = queue_create();
  queue_enqueue(q, ptr1);
  queue_enqueue(q, ptr2);
  queue_enqueue(q, ptr3);
  TEST_ASSERT(queue_length(q) == 3);

  TEST_ASSERT(queue_delete(q, ptr3) == 0);
  TEST_ASSERT(queue_length(q) == 2);

  TEST_ASSERT(queue_delete(q, ptr2) == 0);
  TEST_ASSERT(queue_length(q) == 1);

  TEST_ASSERT(queue_delete(q, ptr1) == 0);
  TEST_ASSERT(queue_length(q) == 0);
  TEST_ASSERT(queue_destroy(q) == 0);
}
static void iterator_inc(queue_t q, void* data)
{
  int* a = (int*)data;

  if (*a == 3)
    queue_delete(q, data);
  else
    *a += 1;
}

/* Test Queue Iterate */
void test_queue_iterate(void)
{
  int val1 = 1;
  int val2 = 2;
  int val3 = 3;
  int* ptr1 = &val1;
  int* ptr2 = &val2;
  int* ptr3 = &val3;

  int* ptr;
  queue_t q;

  fprintf(stderr, "\033[0;33m*** TEST Queue Iterate ***\033[0m\n");

  q = queue_create();
  queue_enqueue(q, ptr1);
  queue_enqueue(q, ptr2);
  queue_enqueue(q, ptr3);
  TEST_ASSERT(queue_length(q) == 3);

  TEST_ASSERT(queue_iterate(q, iterator_inc) == 0);

  TEST_ASSERT(queue_length(q) == 2);
  TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == 0);
  TEST_ASSERT(*ptr == 2);
  TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == 0);
  TEST_ASSERT(*ptr == 3);
}
/* Destroy Null Queue Test */
void test_destroy_null_queue(void)
{
  fprintf(stderr, "\033[0;33m*** TEST Destroy Error ***\033[0m\n");
  queue_t q = NULL;
  TEST_ASSERT(queue_destroy(q) == -1);
}
/* Destroy Full Queue Test*/
void test_destroy_full_queue(void)
{
  int val1 = 1;
  int val2 = 2;
  int val3 = 3;
  int* ptr1 = &val1;
  int* ptr2 = &val2;
  int* ptr3 = &val3;

  queue_t q;

  fprintf(stderr, "\033[0;33m*** TEST Destroy Full Queue ***\033[0m\n");

  q = queue_create();
  queue_enqueue(q, ptr1);
  queue_enqueue(q, ptr2);
  queue_enqueue(q, ptr3);
  TEST_ASSERT(queue_length(q) == 3);

  TEST_ASSERT(queue_destroy(q) == -1);
}
/* Enqueue Null Queue Error Test */
void test_enqueue_null_queue(void)
{
  fprintf(stderr, "\033[0;33m*** TEST Enqueue Null Queue Error ***\033[0m\n");
  queue_t q = NULL;

  int val1 = 1;
  int* ptr1 = &val1;

  TEST_ASSERT(queue_enqueue(q, ptr1) == -1);
}
/* Enqueue Null Data Error Test */
void test_enqueue_null_data(void)
{
  fprintf(stderr, "\033[0;33m*** TEST Enqueue Null Data Error ***\033[0m\n");
  queue_t q = queue_create();

  int* ptr1 = NULL;

  TEST_ASSERT(queue_enqueue(q, ptr1) == -1);
}
/* Dequeue Null Queue Error Test */
void test_dequeue_null_queue(void)
{
  fprintf(stderr, "\033[0;33m*** TEST Dequeue Null Queue Error ***\033[0m\n");
  queue_t q = NULL;

  int val1 = 1;
  void* ptr1 = &val1;

  TEST_ASSERT(queue_dequeue(q, ptr1) == -1);
}
/* Dequeue Null Data Error Test */
void test_dequeue_null_data(void)
{
  fprintf(stderr, "\033[0;33m*** TEST Dequeue Null Data Error ***\033[0m\n");
  queue_t q = queue_create();

  void* ptr1 = NULL;

  TEST_ASSERT(queue_dequeue(q, ptr1) == -1);
}
/* Dequeue Empty Queue Error Test */
void test_dequeue_empty_queue(void)
{
  fprintf(stderr, "\033[0;33m*** TEST Dequeue Empty Queue Error ***\033[0m\n");
  queue_t q = queue_create();

  int val1 = 1;
  void* ptr1 = &val1;

  TEST_ASSERT(queue_dequeue(q, ptr1) == -1);
}
/* Delete Null Queue Error Test */
void test_delete_null_queue(void)
{
  fprintf(stderr, "\033[0;33m*** TEST Delete Null Queue Error ***\033[0m\n");
  queue_t q = NULL;

  int val1 = 1;
  int* ptr1 = &val1;

  TEST_ASSERT(queue_delete(q, ptr1) == -1);
}
/* Delete Null Data Error Test */
void test_delete_null_data(void)
{
  fprintf(stderr, "\033[0;33m*** TEST Delete Null Data Error ***\033[0m\n");
  queue_t q = queue_create();

  int* ptr1 = NULL;

  TEST_ASSERT(queue_delete(q, ptr1) == -1);
}

/* Delete Data Not Found Error Test */
void test_delete_data_not_found(void)
{
  int val1 = 1;
  int val2 = 2;
  int val3 = 3;
  int val4 = 4;
  int* ptr1 = &val1;
  int* ptr2 = &val2;
  int* ptr3 = &val3;
  int* ptr4 = &val4;

  queue_t q;

  fprintf(stderr, "\033[0;33m*** TEST Delete Data Not Found ***\033[0m\n");

  q = queue_create();
  queue_enqueue(q, ptr1);
  queue_enqueue(q, ptr2);
  queue_enqueue(q, ptr3);
  TEST_ASSERT(queue_length(q) == 3);

  TEST_ASSERT(queue_delete(q, ptr4) == -1);
}
/* Iterate Null Queue Error Test */
void test_iterate_null_queue(void)
{
  fprintf(stderr, "\033[0;33m*** TEST Iterate Null Queue Error ***\033[0m\n");
  queue_t q = NULL;

  TEST_ASSERT(queue_iterate(q, iterator_inc) == -1);
}
/* Iterate Null Func Error Test */
void test_iterate_null_func(void)
{
  fprintf(stderr, 
    "\033[0;33m*** TEST Iterate Null Function Error ***\033[0m\n");
  queue_t q = queue_create();

  TEST_ASSERT(queue_iterate(q, NULL) == -1);
}
/* Length Null Queue Test*/
void test_length_null_queue(void)
{
  fprintf(stderr, "\033[0;33m*** TEST Length Null Queue Error ***\033[0m\n");
  queue_t q = NULL;

  TEST_ASSERT(queue_length(q) == -1);
}


int main(void) {
  test_create();
  test_queue_simple();
  test_several_enqueues();
  test_queue_delete();
  test_queue_delete_head();
  test_queue_delete_tail();
  test_queue_iterate();
  test_destroy_null_queue();
  test_destroy_full_queue();
  test_enqueue_null_queue();
  test_enqueue_null_data();
  test_dequeue_null_queue();
  test_dequeue_null_data();
  test_dequeue_empty_queue();
  test_delete_null_queue();
  test_delete_null_data();
  test_delete_data_not_found();
  test_iterate_null_queue();
  test_iterate_null_func();
  test_length_null_queue();

  return 0;
}
