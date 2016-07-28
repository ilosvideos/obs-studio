#include "setup_obs.hpp"

OBSSource setup_video_input(int monitor) {
	std::string name("monitor " + std::to_string(monitor) + " capture");
	OBSSource source = obs_source_create("monitor_capture", name.c_str(), nullptr, nullptr);
	obs_source_release(source);
	{
		obs_data_t * source_settings = obs_data_create();
		obs_data_set_int(source_settings, "monitor", monitor);
		obs_data_set_bool(source_settings, "capture_cursor", false);

		obs_source_update(source, source_settings);
		obs_data_release(source_settings);
	}

	// set this source as output.
	obs_set_output_source(0, source);

	return source;
}

OBSSource setup_audio_input() {
	
	OBSSource source = obs_source_create("wasapi_input_capture", "audio inout", nullptr, nullptr);
	obs_source_release(source);
	{
		obs_data_t * source_settings = obs_data_create();
		//obs_data_set_int(source_settings, "monitor", monitor);
		//obs_data_set_bool(source_settings, "capture_cursor", false);

		obs_source_update(source, source_settings);
		obs_data_release(source_settings);
	}

	// set this source as output.
	obs_set_output_source(1, source);

	return source;
}

Outputs setup_outputs(std::string video_encoder_id, int video_bitrate, std::vector<std::string> output_paths) {
	OBSEncoder video_encoder = obs_video_encoder_create(video_encoder_id.c_str(), "video_encoder", nullptr, nullptr);
	obs_encoder_release(video_encoder);
	obs_encoder_set_video(video_encoder, obs_get_video());
	{
		obs_data_t * encoder_settings = obs_data_create();
		obs_data_set_string(encoder_settings, "rate_control", "CBR");
		obs_data_set_int(encoder_settings, "bitrate", video_bitrate);

		obs_encoder_update(video_encoder, encoder_settings);
		obs_data_release(encoder_settings); 
	}

	OBSEncoder audio_encoder = obs_audio_encoder_create("mf_aac", "audio_encoder", nullptr, 0, nullptr);
	obs_encoder_release(audio_encoder);
	obs_encoder_set_audio(audio_encoder, obs_get_audio());
	{
		obs_data_t * encoder_settings = obs_data_create();
		obs_data_set_string(encoder_settings, "rate_control", "CBR");
		obs_data_set_int(encoder_settings, "samplerate", 44100);
		obs_data_set_int(encoder_settings, "bitrate", 160);
		obs_data_set_default_bool(encoder_settings, "allow he-aac", true);

		obs_encoder_update(audio_encoder, encoder_settings);
		obs_data_release(encoder_settings);
	}


	std::vector<OBSOutput> outputs;
	for (int i = 0; i < output_paths.size(); i++) {
		auto output_path = output_paths[i];
		std::string name("file_output_" + std::to_string(i));
		OBSOutput file_output = obs_output_create("ffmpeg_muxer", name.c_str(), nullptr, nullptr);
		obs_output_release(file_output);
		{
			obs_data_t * output_settings = obs_data_create();
			obs_data_set_string(output_settings, "path", output_path.c_str());
			obs_data_set_string(output_settings, "muxer_settings", NULL);

			obs_output_update(file_output, output_settings);
			obs_data_release(output_settings);
		}

		obs_output_set_video_encoder(file_output, video_encoder);
		obs_output_set_audio_encoder(file_output, audio_encoder, 0);

		outputs.push_back(file_output);
	}

	Outputs out;
	out.video_encoder = video_encoder;
	out.audio_encoder = audio_encoder;
	out.outputs = outputs;
	return out;
}

