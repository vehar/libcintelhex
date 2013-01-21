#include "cintelhex.h"

static void test_can_read_ihex_rs_from_string(char* s);

int  init_parsingsuite(void);
int  clean_parsingsuite(void);
void add_tests_parsingsuite(CU_pSuite suite);

void test_can_read_ihex_rs_from_file_1();
void test_can_read_ihex_rs_from_string_1(void);
void test_no_error_on_correct_checksum(void);
void test_error_on_incorrect_checksum(void);
void test_checksum_is_verified_when_correct(void);
void test_checksum_is_not_verified_when_incorrect(void);
void test_error_on_missing_eof(void);
void test_error_on_incorrect_record_length();

int init_parsingsuite(void)
{
	return 0;
}

int clean_parsingsuite(void)
{
	// ihex_error_reset();
	return 0;
}

void add_tests_parsingsuite(CU_pSuite suite)
{
	CU_add_test(suite, "Record list can be read from file #1", test_can_read_ihex_rs_from_file_1);
	CU_add_test(suite, "Record list can be read from string #1", test_can_read_ihex_rs_from_string_1);
	CU_add_test(suite, "Correct checksum can be verified", test_checksum_is_verified_when_correct);
	CU_add_test(suite, "Incorrect checksum can not be verified", test_checksum_is_not_verified_when_incorrect);
	CU_add_test(suite, "No error is set when checksum is correct", test_no_error_on_correct_checksum);
	CU_add_test(suite, "Error is set when checksum is incorrect", test_error_on_incorrect_checksum);
	CU_add_test(suite, "Error is set when EOF record is missing", test_error_on_missing_eof);
	CU_add_test(suite, "Error is set on incorrect record length", test_error_on_incorrect_record_length);
}

void test_can_read_ihex_rs_from_file_1(void)
{
	ihex_recordset_t* r = ihex_rs_from_file("tests/res/hex1.dat");
	
	CU_ASSERT_PTR_NOT_NULL_FATAL(r);
	CU_ASSERT_EQUAL_FATAL(r->ihrs_count, 6);

	CU_ASSERT_EQUAL(r->ihrs_records[1].ihr_length, 0x10);
	CU_ASSERT_EQUAL(r->ihrs_records[1].ihr_data[0], 0x21);
	CU_ASSERT_EQUAL(r->ihrs_records[1].ihr_type, IHEX_DATA);
	CU_ASSERT_EQUAL(r->ihrs_records[1].ihr_address, 0x0100);
}

void test_can_read_ihex_rs_from_string_1(void)
{
	char* s = ":10010000214601360121470136007EFE09D2190140\r\n:00000001FF\r\n";
	test_can_read_ihex_rs_from_string(s);
}

void test_no_error_on_correct_checksum(void)
{
	char* s = ":10010000214601360121470136007EFE09D2190140\r\n:00000001FF\r\n";
	ihex_recordset_t *records = ihex_rs_from_string(s);

	CU_ASSERT_EQUAL(ihex_errno(), 0);
	CU_ASSERT_PTR_NULL(ihex_error());
}

void test_error_on_missing_eof(void)
{
	char* s = ":10010000214601360121470136007EFE09D2190140\r\n";
	ihex_recordset_t *records = ihex_rs_from_string(s);

	CU_ASSERT_EQUAL(ihex_errno(), IHEX_ERR_NO_EOF);
	CU_ASSERT_PTR_NOT_NULL(ihex_error());
}

void test_error_on_incorrect_checksum(void)
{
	//                                                   v--- Wrong byte!
	char* s = ":10010000214601360121470136007EFE09D2190141\r\n:00000001FF\r\n";
	ihex_recordset_t *records = ihex_rs_from_string(s);

	CU_ASSERT_EQUAL(ihex_errno(), IHEX_ERR_INCORRECT_CHECKSUM);
}

void test_error_on_incorrect_record_length(void)
{
	//                                                v--- Missing byte!
	char* s = ":10010000214601360121470136007EFE09D21940\r\n:00000001FF\r\n";
	ihex_recordset_t *records = ihex_rs_from_string(s);

	CU_ASSERT_EQUAL(ihex_errno(), IHEX_ERR_WRONG_RECORD_LENGTH);
}

void test_checksum_is_verified_when_correct(void)
{
	u_int8_t data[0x10] = {0x21,0x46,0x01,0x36,0x01,0x21,0x47,0x01,0x36,0x00,0x7E,0xFE,0x09,0xD2,0x19,0x01};
	ihex_record_t r = {
		.ihr_length = 0x10, .ihr_type = IHEX_DATA, .ihr_address = 0x0100,
		.ihr_data = (ihex_rdata_t) &data, .ihr_checksum = 0x40
	};
	
	CU_ASSERT_EQUAL(ihex_check_record(&r), 0);
}

void test_checksum_is_not_verified_when_incorrect(void)
{
	u_int8_t data[0x10] = {0x21,0x46,0x01,0x36,0x01,0x21,0x47,0x01,0x36,0x00,0x7E,0xFE,0x09,0xD2,0x19,0x01};
	ihex_record_t r = {
		.ihr_length = 0x10, .ihr_type = IHEX_DATA, .ihr_address = 0x0100,
		.ihr_data = (ihex_rdata_t) &data, .ihr_checksum = 0x20
	};
	
	CU_ASSERT_EQUAL(ihex_check_record(&r), 1);
}

static void test_can_read_ihex_rs_from_string(char* s)
{
	ihex_recordset_t *records = ihex_rs_from_string(s);

	CU_ASSERT_PTR_NOT_NULL_FATAL(records);
	CU_ASSERT_EQUAL_FATAL(records->ihrs_count, 2);

	CU_ASSERT_EQUAL(records->ihrs_records[0].ihr_length, 0x10);
	CU_ASSERT_EQUAL(records->ihrs_records[0].ihr_data[0], 0x21);
	CU_ASSERT_EQUAL(records->ihrs_records[0].ihr_type, IHEX_DATA);
	CU_ASSERT_EQUAL(records->ihrs_records[0].ihr_address, 0x0100);
}
