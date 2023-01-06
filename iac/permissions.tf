#The Service Account used by Pub/Sub needs to have these 2 roles: roles/bigquery.dataEditor and roles/bigquery.metadataViewer

data "google_iam_policy" "admin" {
  binding {
    role = "roles/bigquery.dataEditor"
    members = [
      "serviceAccount:service-${data.google_project.project.number}@gcp-sa-pubsub.iam.gserviceaccount.com",
    ]
  }
  binding {
    role = "roles/bigquery.metadataViewer"
    members = [
      "serviceAccount:service-${data.google_project.project.number}@gcp-sa-pubsub.iam.gserviceaccount.com",
    ]
  }
}

resource "google_bigquery_table_iam_policy" "admin_policy" {
  project = data.google_project.project.project_id
  dataset_id = google_bigquery_table.iot_data_raw.dataset_id
  table_id = google_bigquery_table.iot_data_raw.table_id
  policy_data = data.google_iam_policy.admin.policy_data
}
