/**
* Internal Message Header
* =======================
*
* This header contains the C functions needed for using the message format in the p2p.today project.
*
* It automatically includes :doc:`base.h <./base>`.
*/

#include "./base.h"

#ifdef _cplusplus
extern "C" {
#endif

typedef struct  {
    /**
    * .. c:type:: typedef struct InternalMessageStruct
    *
    *     .. c:member:: char *msg_type
    *
    *         The type of message this is. These are described in :doc:`protocol/flags <../../protocol/flags>`.
    *
    *     .. c:member:: size_t msg_type_len
    *
    *     .. c:member:: char *sender
    *
    *         The message sender's ID
    *
    *     .. c:member:: size_t sender_len
    *
    *     .. c:member:: unsigned long long timestamp
    *
    *         The time at which this message was sent, in UTC seconds since 1/1/1970.
    *
    *     .. c:member:: char **payload
    *
    *         An array of "payload" packets in this message. In other words, every item which *isn't* metadata.
    *
    *     .. c:member:: size_t *payload_lens
    *
    *         The length of each payload item, in the same order
    *
    *     .. c:member:: size_t num_payload
    *
    *         The number of payload packets
    *
    *     .. note::
    *
    *         Members after these are not garunteed to be present. A function to ensure their existence will be
    *         provided as noted. Otherwise check if their length term is ``0`` to determine existence.
    *
    *     .. c:member:: char **compression
    *
    *         An array of possible compression algorigthms
    *
    *         This can be initialized with :c:func:`setInternalMessageCompressions`
    *
    *     .. c:member:: size_t *compression_lens
    *
    *         The length of each compression string, in the same order
    *
    *     .. c:member:: size_t num_compression
    *
    *         The number of compression methods
    *
    *     .. c:member:: char *id
    *
    *         The checksum/ID of this message
    *
    *         To ensure this value is set, call :c:func:`ensureInternalMessageID`
    *
    *     .. c:member:: size_t id_len
    *
    *     .. c:member:: char *str
    *
    *         The serialized version of this message
    *
    *     .. c:member:: size_t str_len
    */
    char *msg_type;
    size_t msg_type_len;
    char *sender;
    size_t sender_len;
    unsigned long long timestamp;
    char **payload;
    size_t *payload_lens;
    size_t num_payload;
    char **compression;
    size_t *compression_lens;
    size_t num_compressions;
    char *compression_used;
    size_t compression_used_len;
    char *id;
    size_t id_len;
    char *str;
    size_t str_len;
} InternalMessageStruct;


static InternalMessageStruct *constructInternalMessage(const char *type, size_t type_len, const char *sender, size_t sender_len, char **payload, size_t *payload_lens, size_t num_payload)   {
    /**
    * .. c:function:: static InternalMessageStruct *constructInternalMessage(const char *type, size_t type_len, const char *sender, size_t sender_len, char **payload, size_t *payload_lens, size_t num_payload)
    *
    *     Constructs an InternalMessageStruct. This copies all given data into a struct, then returns this struct's pointer.
    *
    *     :param type:          The item to place in :c:member:`InternalMessageStruct.msg_type`
    *     :param type_len:      The length of the above
    *     :param sender:        The item to place in :c:member:`InternalMessageStruct.sender`
    *     :param sender_len:    The length of the above
    *     :param payload:       The array to place in :c:member:`InternalMessageStruct.payload`
    *     :param payload_lens:  The length for each string in the above
    *     :param num_payload:   The number of items in the above
    *
    *     :returns: A pointer to the resulting :c:type:`InternalMessageStruct`
    *
    *     .. warning::
    *
    *          You must use :c:func:`destroyInternalMessage` on the resulting object, or you will develop a memory leak
    */
    CP2P_DEBUG("Inside real constructor. num_payload=%i\n", num_payload);
    InternalMessageStruct *ret = (InternalMessageStruct *) malloc(sizeof(InternalMessageStruct));
    ret->msg_type = (char *) malloc(sizeof(char) * type_len);
    memcpy(ret->msg_type, type, type_len);
    ret->msg_type_len = type_len;
    ret->sender = (char *) malloc(sizeof(char) * sender_len);
    memcpy(ret->sender, sender, sender_len);
    ret->sender_len = sender_len;
    ret->timestamp = getUTC();
    ret->num_payload = num_payload;
    ret->payload = (char **) malloc(sizeof(char *) * num_payload);
    ret->payload_lens = (size_t *) malloc(sizeof(size_t) * num_payload);
    CP2P_DEBUG("At for loop\n");
    size_t i;
    for (i = 0; i < num_payload; i++)    {
        ret->payload[i] = (char *) malloc(sizeof(char) * payload_lens[i]);
        ret->payload_lens[i] = payload_lens[i];
        memcpy(ret->payload[i], payload[i], payload_lens[i]);
        CP2P_DEBUG("%s\n", ret->payload[i]);
    }
    CP2P_DEBUG("Exited for loop\n");
    ret->compression = NULL;
    ret->compression_lens = NULL;
    ret->num_compressions = 0;
    ret->compression_used = NULL;
    ret->compression_used_len = 0;
    ret->id = NULL;
    ret->id_len = 0;
    ret->str = NULL;
    ret->str_len = 0;
    CP2P_DEBUG("Returning\n");
    return ret;
}

static void destroyInternalMessage(InternalMessageStruct *des)    {
    /**
    * .. c:function:: static void destroyInternalMessage(InternalMessageStruct *des)
    *
    *     :c:func:`free` an :c:type:`InteralMessageStruct` and its members
    *
    *     :param des: A pointer to the InternalMessageStruct you wish to destroy
    */
    CP2P_DEBUG("1\n");
    free(des->msg_type);
    CP2P_DEBUG("2\n");
    size_t i;
    for (i = 0; i < des->num_payload; i++)   {
        free(des->payload[i]);
    }
    CP2P_DEBUG("3\n");
    free(des->payload);
    CP2P_DEBUG("4\n");
    free(des->payload_lens);
    CP2P_DEBUG("5\n");
    if (des->compression != NULL)   {
        CP2P_DEBUG("6\n");
        for (i = 0; i < des->num_compressions; i++)  {
            free(des->compression[i]);
        }
        CP2P_DEBUG("7\n");
        free(des->compression);
        CP2P_DEBUG("8\n");
        free(des->compression_lens);
    }
    CP2P_DEBUG("9\n");
    if (des->id != NULL)    {
        free(des->id);
    }
    CP2P_DEBUG("10\n");
    if (des->str != NULL)   {
        free(des->str);
    }
    CP2P_DEBUG("11\n");
    free(des);
}

static void setInternalMessageCompressions(InternalMessageStruct *des, char **compression, size_t *compression_lens, size_t num_compressions)   {
    /**
    * .. c:function:: static void setInternalMessageCompressions(InternalMessageStruct *des, char **compression, size_t *compression_lens, size_t num_compressions)
    *
    *     Sets the compression methods for a particular :c:type:`InternalMessageStruct`. These methods are formatted as an array of strings, an array of lengths, and a
    *     number of methods. The data is copied, so you inputs can be local variables.
    *
    *     :param des:               A pointer to the relevant InternalMessageStruct
    *     :param compression:       An array of compression methods
    *     :param compression_lens:  An array of lengths for each compression method
    *     :param num_compressions:  The number of compression methods
    */
    size_t i;
    if (des->compression != NULL)   {
        for (i = 0; i < des->num_compressions; i++)  {
            free(des->compression[i]);
        }
        free(des->compression);
        free(des->compression_lens);
    }
    des->compression = (char **) malloc(sizeof(char *) * num_compressions);
    des->compression_lens = (size_t *) malloc(sizeof(size_t) * num_compressions);
    for (i = 0; i < des->num_compressions; i++)  {
        des->compression[i] = (char *) malloc(sizeof(char) * compression_lens[i]);
        memcpy(des->compression[i], compression[i], compression_lens[i]);
    }
}

static void ensureInternalMessageID(InternalMessageStruct *des)  {
    /**
    * .. c:function:: static void ensureInternalMessageID(InternalMessageStruct *des)
    *
    *     Ensures that the InternalMessageStruct has an ID calculated and assigned
    *
    *     :param des: A pointer to the relevant InternalMessageStruct
    */
    if (des->id != NULL)   {
        CP2P_DEBUG("ID already exists\n")
        return;
    }

    unsigned char digest[SHA384_DIGEST_LENGTH];
    memset(digest, 0, SHA384_DIGEST_LENGTH);
    SHA384_CTX ctx;
    SHA384_Init(&ctx);

    size_t i;
    for (i = 0; i < des->num_payload; i++)
        SHA384_Update(&ctx, (const unsigned char *) des->payload[i], des->payload_lens[i]);

    size_t t58_len = 0;
    char *t58 = to_base_58(des->timestamp, &t58_len);
    SHA384_Update(&ctx, (const unsigned char *) t58, t58_len);
    free(t58);
    SHA384_Final(digest, &ctx);
    des->id = ascii_to_base_58((const char *)digest, (size_t) SHA384_DIGEST_LENGTH, &(des->id_len), 1);
}

static void ensureInternalMessageStr(InternalMessageStruct *des) {
    /**
    * .. c:function:: static void ensureInternalMessageStr(InternalMessageStruct *des)
    *
    *     Ensures that the InternalMessageStruct has a serialized string calculated and assigned
    *
    *     :param des: A pointer to the relevant InternalMessageStruct
    */
    if (des->str != NULL)   {
        CP2P_DEBUG("str already exists\n")
        return;
    }

    ensureInternalMessageID(des);
    size_t num_packets = 5 + des->num_payload;
    size_t expected = des->msg_type_len + des->sender_len + des->id_len;
    char *str = (char *) malloc(sizeof(char) * 4 * num_packets);
    pack_value(4, str + 4, des->msg_type_len);
    pack_value(4, str + 8, des->sender_len);
    pack_value(4, str + 12, des->id_len);

    size_t t58_len = 0;
    char *t58 = to_base_58(des->timestamp, &t58_len);
    pack_value(4, str + 16, t58_len);
    expected += t58_len;

    size_t i;
    for (i = 0; i < des->num_payload; i++)   {
        pack_value(4, str + 20 + 4 * i, des->payload_lens[i]);
        expected += des->payload_lens[i];
    }

    size_t processed = 4 * num_packets;
    str = (char *) realloc(str, sizeof(char) * (processed + expected));

    memcpy(str + processed, des->msg_type, des->msg_type_len);
    processed += des->msg_type_len;
    memcpy(str + processed, des->sender, des->sender_len);
    processed += des->sender_len;
    memcpy(str + processed, des->id, des->id_len);
    processed += des->id_len;
    memcpy(str + processed, t58, t58_len);
    processed += t58_len;
    free(t58);

    for (i = 0; i < des->num_payload; i++)   {
        memcpy(str + processed, des->payload[i], des->payload_lens[i]);
        processed += des->payload_lens[i];
    }
    des->str_len = processed;
    pack_value(4, str, processed - 4);
    des->str = str;
}

static InternalMessageStruct *deserializeInternalMessage(const char *serialized, size_t len, int sizeless, int *errored)  {
    /**
    * .. c:function:: static InternalMessageStruct *deserializeInternalMessage(const char *serialized, size_t len, int sizeless)
    *
    *     Deserializes an uncompressed :c:type:`InternalMessageStruct`. The ``sizeless`` parameter indicates whether the network size
    *     header is still present on the given string.
    *
    *     :param serialized:    The serialized message
    *     :param len:           The length of the serialized message
    *     :param sizeless:      A boolean which indicates whether the network size header is still present on the given string
    *     :param errored:       A pointer to a boolean. If this is set with a non-zero value, it indicates that the checksum test failed
    *
    *     :returns: An equivalent :c:type:`InternalMessageStruct`
    */
    char *tmp = (char *) malloc(sizeof(char) * len);
    memcpy(tmp, serialized, len);
    sanitize_string(tmp, &len, sizeless);
    char **packets = (char **) malloc(sizeof(char *) * 4);
    size_t *lens;
    size_t num_packets;
    CP2P_DEBUG("Entering process_string\n");
    process_string(tmp, len, &packets, &lens, &num_packets);
    CP2P_DEBUG("Exiting process_string\n");
    InternalMessageStruct *ret = constructInternalMessage(
        packets[0], lens[0],
        packets[1], lens[1],
        packets + 4, lens + 4, num_packets - 4);
    ret->timestamp = from_base_58(packets[3], lens[3]);
    ret->str_len = len;
    ret->str = (char *) malloc(sizeof(char) * len);
    memcpy(ret->str, serialized, len);

    ensureInternalMessageID(ret);
    *errored = memcmp(ret->id, packets[2], ret->id_len);

    return ret;
}

static InternalMessageStruct *deserializeCompressedInternalMessage(const char *serialized, size_t len, int sizeless, int *errored, char **compression, size_t *compression_lens, size_t num_compressions)    {
    /**
    * .. c:function:: static InternalMessageStruct *deserializeCompressedInternalMessage(const char *serialized, size_t len, int sizeless, char **compression, size_t *compression_lens, size_t num_compressions)
    *
    *     Deserializes a compressed :c:type:`InternalMessageStruct`. The ``sizeless`` parameter indicates whether the network size
    *     header is still present on the given string.
    *
    *     :param serialized:        See :c:func:`deserializeInternalMessage`
    *     :param len:               See :c:func:`deserializeInternalMessage`
    *     :param sizeless:          See :c:func:`deserializeInternalMessage`
    *     :param errored:           See :c:func:`deserializeInternalMessage`
    *     :param compression:       See :c:func:`setInternalMessageCompressions`
    *     :param compression_lens:  See :c:func:`setInternalMessageCompressions`
    *     :param num_compressions:  See :c:func:`setInternalMessageCompressions`
    *
    *     :returns: An equivalent :c:type:`InternalMessageStruct`
    */
    char *tmp = (char *) malloc(sizeof(char) * len);
    memcpy(tmp, serialized, len);
    sanitize_string(tmp, &len, sizeless);
    char *result;
    size_t res_len;
    decompress_string(tmp, len, &result, &res_len, compression, compression_lens, num_compressions);
    InternalMessageStruct *ret = deserializeInternalMessage(result, res_len, 0, errored);
    setInternalMessageCompressions(ret, compression, compression_lens, num_compressions);
    return ret;
}

#ifdef _cplusplus
}
#endif
