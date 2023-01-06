resource "google_bigquery_dataset" "iot_data" {
  dataset_id                  = "iotdata"
  friendly_name               = "IoT data"
  description                 = "Where IoT data gets stored"
  location                    = var.bigquery_location
}

resource "google_bigquery_table" "iot_data_raw" {
  dataset_id = google_bigquery_dataset.iot_data.dataset_id
  table_id   = "iot-data-raw"

  schema = <<EOF
    [
        {
            "mode": "NULLABLE",
            "name": "subscription_name",
            "type": "STRING"
        },
        {
            "mode": "NULLABLE",
            "name": "message_id",
            "type": "STRING"
        },
        {
            "mode": "NULLABLE",
            "name": "publish_time",
            "type": "TIMESTAMP"
        },
        {
            "mode": "NULLABLE",
            "name": "data",
            "type": "STRING"
        },
        {
            "mode": "NULLABLE",
            "name": "attributes",
            "type": "JSON"
        }
    ]
    EOF
}