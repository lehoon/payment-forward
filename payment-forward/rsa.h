#ifndef _PAYMENT_OPENSSL_RSA_H_
#define _PAYMENT_OPENSSL_RSA_H_

#include <string>
#include <openssl/rsa.h>

namespace security {

	std::string base64_encode(const std::string&);
	std::string base64_decode(const std::string&);

	//公钥加密
	std::string rsa_public_key_encrypt(const std::string& public_key, const std::string& in);
	//公钥解密
	std::string rsa_public_key_decrypt(const std::string& public_key, const std::string& in);

	//私钥加密
	std::string rsa_private_key_encrypt(const std::string& private_key, const std::string& in);
	//私钥解密
	std::string rsa_private_key_decrypt(const std::string& private_key, const std::string& in);

	//字符串转公钥
	RSA* string_to_public_key(std::string public_key);
	//字符串转私钥
	RSA* string_to_private_key(std::string private_key);

};



#endif // _PAYMENT_OPENSSL_RSA_H_