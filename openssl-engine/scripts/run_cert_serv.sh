#!/bin/bash
# With NEW_KEY=1 the script requires to enter the password 3 times. 
# It is required because by default "openssl req"
# cannot do it without the password. 
# Then "openssl ec" removes the password
# NOTE! Be careful with NEW_KEY=1. Atmel certificates may be invalidated!
# A new key may be generated if key is not locked! 

set -e
set -x
cd $(dirname $0)
source ./common.sh

cd ..
cd ${CERTSTORE}

if [ $USE_WWW = "0" ]; then
    EXTENSION=v3_ca
    ECCX08_CN="_eccx08"
else
    EXTENSION=usr_cert
    ECCX08_CN=
fi

if [ $NEW_ROOT = "1" ]; then
    ##
    ## Root cert
    ##
    ${CMD} ecparam -name prime256v1 -out prime256v1.pem

    ${CMD} ecparam -out ${CERTSTORE}/privkeys/${COMPANY}_root.key \
          -name prime256v1 -genkey
    ${CMD} req -new -key ${CERTSTORE}/privkeys/${COMPANY}_root.key \
          -out ${CERTSTORE}/csr/${COMPANY}_root.csr -sha256 \
	  -config ${CERTSTORE}/openssl.cnf \
	  -subj "/C=US/ST=CA/L=Sunnyvale/O=Homut LLC/CN=Homut Root/" \
	  -verify
    ${CMD} ca -batch \
          -create_serial -out ${CERTSTORE}/trusted/${COMPANY}_root.crt \
          -days 1000 -selfsign -extensions v3_ca_has_san -config ${CERTSTORE}/openssl.cnf \
	  -keyfile ${CERTSTORE}/privkeys/${COMPANY}_root.key \
	  -infiles ${CERTSTORE}/csr/${COMPANY}_root.csr

    ##
    ## Intermediate cert
    ##
    ${CMD} ecparam -out ${CERTSTORE}/privkeys/${COMPANY}_intermediate.key \
          -name prime256v1 -genkey
    ${CMD} req -new -key ${CERTSTORE}/privkeys/${COMPANY}_intermediate.key \
          -out ${CERTSTORE}/csr/${COMPANY}_intermediate.csr -sha256 \
	  -config ${CERTSTORE}/openssl.cnf \
	  -subj "/C=US/ST=CA/L=Sunnyvale/O=Homut LLC/CN=homut_intermediate/" \
	  -verify
    ${CMD} ca -batch \
          -config ${CERTSTORE}/openssl.cnf -extensions v3_ca \
          -days 365 -in ${CERTSTORE}/csr/${COMPANY}_intermediate.csr \
	  -cert ${CERTSTORE}/trusted/${COMPANY}_root.crt \
	  -keyfile ${CERTSTORE}/privkeys/${COMPANY}_root.key \
	  -out ${CERTSTORE}/trusted/${COMPANY}_intermediate.crt

    ##
    ## Create cert bundle
    ##
    cat ${CERTSTORE}/trusted/${COMPANY}_root.crt \
     ${CERTSTORE}/trusted/${COMPANY}_intermediate.crt > \
     ${CERTSTORE}/trusted/${COMPANY}_bundle.crt

fi

if [ $NEW_KEY = "1" ]; then

    #  generate a new key then generate and sign server certificate using engine
    rm -f ${CERTSTORE}/privkeys/${COMPANY}_server_eccx08.key \
	${CERTSTORE}/csr/${COMPANY}_server_eccx08.csr \
	${CERTSTORE}/personal/${COMPANY}_server_eccx08.crt \
	${CERTSTORE}/privkeys/${COMPANY}_server_eccx08.der

    ${CMD} req ${KEYGEN_ENGINE} \
	-new -newkey ec:${CERTSTORE}/prime256v1.pem \
	-keyout ${CERTSTORE}/privkeys/${COMPANY}_server_eccx08.key \
	-out ${CERTSTORE}/csr/${COMPANY}_server_eccx08.csr \
	-sha256 -config ${CERTSTORE}/openssl.cnf \
	-subj /C=US/ST=CA/L=Sunnyvale/O=Homut\ LLC/CN=${COMMON_NAME}${ECCX08_CN}/ \
	-verify

    #  generate a new key then generate and sign server certificate without using engine
    ${CMD} ecparam -out ${CERTSTORE}/privkeys/${COMPANY}_server.key \
	-name prime256v1 -genkey
    ${CMD} req -new -key ${CERTSTORE}/privkeys/${COMPANY}_server.key \
	-out ${CERTSTORE}/csr/${COMPANY}_server.csr -sha256 \
	-config ${CERTSTORE}/openssl.cnf \
	-subj /C=US/ST=CA/L=Sunnyvale/O=Homut\ LLC/CN=${COMMON_NAME}/ \
	-verify
    ${CMD} ca -batch \
	-config ${CERTSTORE}/openssl.cnf -extensions ${EXTENSION} \
	-days 365 -in ${CERTSTORE}/csr/${COMPANY}_server.csr \
	-cert ${CERTSTORE}/trusted/${COMPANY}_intermediate.crt \
	-keyfile ${CERTSTORE}/privkeys/${COMPANY}_intermediate.key \
	-out ${CERTSTORE}/personal/${COMPANY}_server.crt

else

    #  generate and sign certificates using engine, use existing key
    ${CMD} req ${ENGINE} \
	-new -key ${CERTSTORE}/privkeys/${COMPANY}_server_eccx08.key \
	-out ${CERTSTORE}/csr/${COMPANY}_server_eccx08.csr \
	-sha256 -config ${CERTSTORE}/openssl.cnf \
	-subj /C=US/ST=CA/L=Sunnyvale/O=Homut\ LLC/CN=${COMMON_NAME}${ECCX08_CN}/ \
	-verify

fi

${CMD} ca -batch \
    -config ${CERTSTORE}/openssl.cnf \
    -extensions ${EXTENSION} -days 365 \
    -outdir ${CERTSTORE}/newcerts \
    -in ${CERTSTORE}/csr/${COMPANY}_server_eccx08.csr \
    -cert ${CERTSTORE}/trusted/${COMPANY}_intermediate.crt \
    -keyfile ${CERTSTORE}/privkeys/${COMPANY}_intermediate.key \
    -out ${CERTSTORE}/personal/${COMPANY}_server_eccx08.crt

${CMD} ec \
    -in ${CERTSTORE}/privkeys/${COMPANY}_server_eccx08.key \
    -out ${CERTSTORE}/privkeys/${COMPANY}_server_eccx08.key

${CMD} ec \
    -in ${CERTSTORE}/privkeys/${COMPANY}_server_eccx08.key \
    -outform DER -out ${CERTSTORE}/privkeys/${COMPANY}_server_eccx08.der

hexdump -C ${CERTSTORE}/privkeys/${COMPANY}_server_eccx08.der

exit $?
