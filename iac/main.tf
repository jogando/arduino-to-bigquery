terraform {
 backend "gcs" {
   prefix  = "terraform/state"
 }
}

provider "google" {
  project     = var.project_name
  region      = var.region
}

data "google_project" "project" {
}

# Pub/Sub topic required by IoT Core 
resource "google_pubsub_topic" "iot_data" {
  name = "iot-data"

  message_retention_duration = "3600s"
}

resource "google_cloudiot_registry" "iot_registry" {
  name     = "cloudiot-registry"

  log_level = "DEBUG"

  event_notification_configs {
    pubsub_topic_name = google_pubsub_topic.iot_data.id
    subfolder_matches = ""
  }
}

#Redirect the contents of the Pub/Sub subscription into the Bigquery table
resource "google_pubsub_subscription" "pubsub_to_bigquery" {
    name  = "pubsub_to_bigquery"
    topic = google_pubsub_topic.iot_data.name

    bigquery_config {
        write_metadata = true
        table = "${google_bigquery_table.iot_data_raw.project}:${google_bigquery_table.iot_data_raw.dataset_id}.${google_bigquery_table.iot_data_raw.table_id}"
    }

    depends_on = [google_bigquery_table_iam_policy.admin_policy]
}