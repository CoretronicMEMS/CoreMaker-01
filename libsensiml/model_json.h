#ifndef __MODEL_JSON_H__
#define __MODEL_JSON_H__

const char recognition_model_string_json[] = {"{\"NumModels\":1,\"ModelIndexes\":{\"0\":\"AIoT_acc_test_rank_0\"},\"ModelDescriptions\":[{\"Name\":\"AIoT_acc_test_rank_0\",\"ClassMaps\":{\"1\":\"anomaly\",\"2\":\"normal\",\"0\":\"Unknown\"},\"ModelType\":\"PME\",\"FeatureFunctions\":[\"AbsoluteAreaofSpectrum\"]}]}"};

int recognition_model_string_json_len = sizeof(recognition_model_string_json);

#endif /* __MODEL_JSON_H__ */
