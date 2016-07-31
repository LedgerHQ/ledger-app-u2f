/*
*******************************************************************************
*   Portable FIDO U2F implementation
*   Ledger Blue specific initialization
*   (c) 2016 Ledger
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*   limitations under the License.
********************************************************************************/

#include "os.h"
#include "u2f_config.h"

WIDE u2f_config_t const N_u2f;

void u2f_init_config(void) {
    if (N_u2f.initialized != 1) { // TODO use a magic here ?, well the memory is
                                  // wiped prior to install but still
        u2f_config_t u2fConfig;
        u2fConfig.counter = 1;
        u2fConfig.initialized = 1;
#ifndef DERIVE_JOHOE
        uint32_t keyPath[1];
        keyPath[0] = U2F_KEY_PATH;
        os_perso_derive_seed_bip32(keyPath, 1, u2fConfig.hmacKey,
                                   u2fConfig.hmacKey + 32);
#endif
        nvm_write(&N_u2f, &u2fConfig, sizeof(u2f_config_t));
    }
#ifndef DERIVE_JOHOE
    else {
        // Check that the seed did not change - if it did, overwrite the hmac
        // key
        u2f_config_t u2fConfig;
        uint32_t keyPath[1];
        os_memmove(&u2fConfig, &N_u2f, sizeof(u2f_config_t));
        keyPath[0] = U2F_KEY_PATH;
        os_perso_derive_seed_bip32(keyPath, 1, u2fConfig.hmacKey,
                                   u2fConfig.hmacKey + 32);
        if (os_memcmp(u2fConfig.hmacKey, N_u2f.hmacKey,
                      sizeof(u2fConfig.hmacKey)) != 0) {
            nvm_write(N_u2f.hmacKey, u2fConfig.hmacKey,
                      sizeof(u2fConfig.hmacKey));
        }
    }
#endif
}