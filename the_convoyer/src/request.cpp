#include "request.h"
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <M5Stack.h>

const String baseUrl = "https://theconvoyerepitech.doli.bar/api/index.php";
const String apiKey = "5bYU8uxA2nTNaEwFxm6O6Ne7y8O8G9u9";

String httpGETRequest(const String& serverName) {
    HTTPClient http;
    http.begin(serverName);

    int httpResponseCode = http.GET();

    String payload = "{}";

    if (httpResponseCode>0) {
        M5.Lcd.println("HTTP Response code: ");
        M5.Lcd.println(httpResponseCode);
        payload = http.getString();
    }
    else {
        M5.Lcd.println("Error code: ");
        M5.Lcd.println(httpResponseCode);
    }

    http.end();

    return payload;
}

String httpPOSTRequest(String serverName, String data) {
    M5.Lcd.println(data);
    HTTPClient http;
    http.begin(std::move(serverName));
    http.addHeader("Content-Type","application/json");

    int httpResponseCode = http.POST(std::move(data));

    String payload = "{}";

    if (httpResponseCode>0) {
        M5.Lcd.println("HTTP Response code: ");
        M5.Lcd.println(httpResponseCode);
        payload = http.getString();
    }
    else {
        M5.Lcd.println("Error code: ");
        M5.Lcd.println(httpResponseCode);
    }

    http.end();

    return payload;
}

JSONVar getProductByRef(const String& ref) {
    String url = baseUrl + "/products/ref/" + ref + "?DOLAPIKEY=" + apiKey;
    String response = httpGETRequest(url);
    JSONVar myObject = JSON.parse(response);

    return myObject;
};

JSONVar getWarehouseById(const String& id) {
    String url = baseUrl + "/warehouses/" + id + "?DOLAPIKEY=" + apiKey;
    String response = httpGETRequest(url);
    JSONVar myObject = JSON.parse(response);

    return myObject;
}

JSONVar createStockMovements(const int productId, const int wareHouseId) {
    JSONVar data;
    String url = baseUrl + "/stockmovements?DOLAPIKEY=" + apiKey;
    data["product_id"] = productId;
    data["warehouse_id"] = wareHouseId;
    data["qty"] = 1;

    String response = httpPOSTRequest(url, JSON.stringify(data));
    return response;
}

JSONVar createProduct(const String& ref, const String& label, const int wareHouseId) {
    JSONVar data;
    String url = baseUrl + "/products?DOLAPIKEY=" + apiKey;

    data["ref"] = ref;
    data["label"] = label;
    data["fk_default_warehouse"] = wareHouseId;

    String response = httpPOSTRequest(url, JSON.stringify(data));
    JSONVar object = JSON.parse(response);
    return object;
};