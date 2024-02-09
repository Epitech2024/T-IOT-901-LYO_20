#ifndef THE_CONVOYER_REQUEST_H
#define THE_CONVOYER_REQUEST_H
#include <HTTPClient.h>
#include <Arduino_JSON.h>

JSONVar getProductByRef(const String& ref);

JSONVar getWarehouseById(const String& id);

JSONVar createStockMovements(const int productId, const int wareHouseId);

JSONVar createProduct(const String& ref, const String& label, int wareHouseId);

#endif //THE_CONVOYER_REQUEST_H
