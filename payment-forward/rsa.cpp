#include "rsa.h"

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <cassert>

#include <string>
#include <iostream>

namespace security {

	//公钥加密
	std::string rsa_public_key_encrypt(const std::string& public_key, const std::string& in) {
		if (public_key.empty() || in.empty())
		{
			assert(false);
			return "";
		}

		std::string strRet;
		RSA* pRSAPublicKey = string_to_public_key(public_key.c_str());
		if (pRSAPublicKey == NULL)
		{
			assert(false);
			return "";
		}

		int nLen = RSA_size(pRSAPublicKey);
		char* pEncode = new char[nLen + 1];

		int ret = RSA_public_encrypt(in.length(), (const unsigned char*)in.c_str(), (unsigned char*)pEncode, pRSAPublicKey, RSA_PKCS1_PADDING/* RSA_NO_PADDING  RSA_PKCS1_PADDING */);
		if (ret >= 0)
		{
			strRet = std::string(pEncode, ret);
		}

		delete[] pEncode;
		RSA_free(pRSAPublicKey);
		CRYPTO_cleanup_all_ex_data();
		return strRet;
	}

	//公钥解密
	std::string rsa_public_key_decrypt(const std::string& public_key, const std::string& in) {
		if (public_key.empty() || in.empty())
		{
			assert(false);
			return "";
		}

		std::string strRet;
		RSA* pRSAPublicKey = string_to_public_key(public_key.c_str());
		if (pRSAPublicKey == NULL)
		{
			assert(false);
			return "";
		}

		int nLen = RSA_size(pRSAPublicKey);
		char* pDecode = new char[nLen + 1];

		int ret = RSA_public_decrypt(in.length(), (const unsigned char*)in.c_str(), (unsigned char*)pDecode, pRSAPublicKey, RSA_PKCS1_PADDING/* RSA_NO_PADDING  RSA_PKCS1_PADDING */);
		if (ret >= 0)
		{
			strRet = std::string((char*)pDecode, ret);
		}

		delete[] pDecode;
		RSA_free(pRSAPublicKey);
		CRYPTO_cleanup_all_ex_data();
		return strRet;
	}

	//私钥加密
	std::string rsa_private_key_encrypt(const std::string& private_key, const std::string& in) {
		if (private_key.empty() || in.empty())
		{
			assert(false);
			return "";
		}

		std::string strRet;
		RSA* pRSAPrivateKey = string_to_private_key(private_key.c_str());
		if (pRSAPrivateKey == NULL)
		{
			assert(false);
			return "";
		}

		int nLen = RSA_size(pRSAPrivateKey);
		char* pEncode = new char[nLen + 1];

		int ret = RSA_private_encrypt(in.length(), (const unsigned char*)in.c_str(), (unsigned char*)pEncode, pRSAPrivateKey, RSA_PKCS1_PADDING/* RSA_NO_PADDING  RSA_PKCS1_PADDING */);
		if (ret >= 0)
		{
			strRet = std::string(pEncode, ret);
		}

		delete[] pEncode;
		RSA_free(pRSAPrivateKey);
		CRYPTO_cleanup_all_ex_data();
		return strRet;
	}

	//私钥解密
	std::string rsa_private_key_decrypt(const std::string& private_key, const std::string& in) {
		if (private_key.empty() || in.empty())
		{
			assert(false);
			return "";
		}

		std::string strRet;
		RSA* pRSAPriKey = string_to_private_key(private_key.c_str());
		if (pRSAPriKey == NULL)
		{
			assert(false);
			return "";
		}

		int nLen = RSA_size(pRSAPriKey);
		char* pDecode = new char[nLen + 1];

		int ret = RSA_private_decrypt(in.length(), (const unsigned char*)in.c_str(), (unsigned char*)pDecode, pRSAPriKey, RSA_PKCS1_PADDING/* RSA_NO_PADDING  RSA_PKCS1_PADDING */);
		if (ret >= 0)
		{
			strRet = std::string((char*)pDecode, ret);
		}

		delete[] pDecode;
		RSA_free(pRSAPriKey);
		CRYPTO_cleanup_all_ex_data();
		return strRet;
	}

	//字符串转公钥
	RSA* string_to_public_key(std::string public_key) {
		int n_len = public_key.size();      //public_key为base64编码的公钥字符串
		for (int i = 64; i < n_len; i += 64)
		{
			if (public_key[i] != '\n')
			{
				public_key.insert(i, "\n");
			}
			i++;
		}

		public_key.insert(0, "-----BEGIN PUBLIC KEY-----\n");
		public_key.append("\n-----END PUBLIC KEY-----\n");

		BIO* bio = NULL;
		RSA* rsa = NULL;
		char* chPublicKey = const_cast<char*>(public_key.c_str());
		if ((bio = BIO_new_mem_buf(chPublicKey, -1)) == NULL)       //从字符串读取RSA公钥
		{
			std::cout << "BIO_new_mem_buf failed!" << std::endl;
		}
		else
		{
			rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);   //从bio结构中得到rsa结构
			if (!rsa)
			{
				ERR_load_crypto_strings();
				char errBuf[512];
				ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
				std::cout << "fomat public key failed[" << errBuf << "]" << std::endl;
				BIO_free_all(bio);
			}
		}

		return rsa;
	}

	//字符串转私钥
	RSA* string_to_private_key(std::string private_key) {
		int n_len = private_key.size();      //private_key为base64编码的公钥字符串
		for (int i = 64; i < n_len; i += 64)
		{
			if (private_key[i] != '\n')
			{
				private_key.insert(i, "\n");
			}
			i++;
		}

		private_key.insert(0, "-----BEGIN RSA PRIVATE KEY-----\n");
		private_key.append("\n-----END RSA PRIVATE KEY-----\n");

		BIO* bio = NULL;
		RSA* rsa = NULL;
		char* chPrivateKey = const_cast<char*>(private_key.c_str());
		if ((bio = BIO_new_mem_buf(chPrivateKey, -1)) == NULL)       //从字符串读取RSA公钥
		{
			std::cout << "BIO_new_mem_buf failed!" << std::endl;
		}
		else
		{
			rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);   //从bio结构中得到rsa结构
			if (!rsa)
			{
				ERR_load_crypto_strings();
				char errBuf[512];
				ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
				std::cout << "fomat public key failed[" << errBuf << "]" << std::endl;
				BIO_free_all(bio);
			}
		}

		return rsa;
	}

};