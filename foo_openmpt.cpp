#define BUILD_VERSION ""
//#define BUILD_VERSION "+1"

/*
	change log

2018-10-03 07:32 UTC - kode54
- Updated libopenmpt to version 0.3.12 with bug fixes and minor feature changes.
- Version is now 0.3.12

2018-07-29 02:10 UTC - kode54
- Updated libopenmpt to version 0.3.11 with security and bug fixes.
- Version is now 0.3.11

2018-06-19 02:29 UTC - kode54
- Updated libopenmpt to version 0.3.10 with security and bug fixes.
- Added a minimum song length check, to help eliminate subsong pollution.
- Version is now 0.3.10

2018-05-02 03:08 UTC - kode54
- Updated libopenmpt to version 0.3.9 with security and bug fixes.
- Version is now 0.3.9

2018-04-09 23:38 UTC - kode54
- Updated libopenmpt to version 0.3.8 with two releases worth of security and
  bug fixes.
- Version is now 0.3.8

2018-02-05 08:41 UTC - kode54
- Updated libopenmpt to version 0.3.6 with important security and bug fixes,
  and new features.
- Version is now 0.3.6

2018-01-29 23:29 UTC - kode54
- Updated file info reading to not return empty strings
- Version is now 0.3.5+1

2018-01-29 02:11 UTC - kode54
- Switched libopenmpt from unstable SVN snapshot to stable release, for
  everyone's benefit
- Implemented font deriving from UI element default font, still hard
  coded to use Lucida Console. Expect a better font solution in the future.
  This also should make the dialog DPI aware.
- Version is now 0.3.5 - force deprecating 0.4.0-pre.*

2018-01-19 04:08 UTC - kode54
- Implemented std::shared_ptr instead of stupid reference counted crap
  for openmpt handles, better accounting for shared pointer use everywhere
- Implemented OpenMPT control dialog, based on foo_dumb's control dialog,
  only this time it also has tempo and pitch controls
- Version is now 0.4.0-pre.3+6

2018-01-16 23:36 UTC - kode54
- Implemented double buffering for flicker-free pattern visualization
- Version is now 0.4.0-pre.3+3

2018-01-16 23:35 UTC - kode54
- Implemented conventional preferences dialog
- Implemented first version of visualization, based on the OpenMPT
  team's visualizer from xmp-openmpt
- Version is now 0.4.0-pre.3+2

2018-01-15 21:07 UTC - kode54
- Implemented dynamic track information reporting
- Implemented numbered sample, instrument, etc info reporting
- Version is now 0.4.0-pre.3+1

2018-01-25 02:28 UTC - kode54
- Initial release
- Version is now 0.4.0-pre.3

*/

#if defined(_MSC_VER)
#pragma warning(disable:4091)
#endif

#include <foobar2000.h>
#include "../helpers/dropdown_helper.h"
#include "../helpers/window_placement_helper.h"
#include "../ATLHelpers/ATLHelpersLean.h"
#include "../ATLHelpers/misc.h"

#include <WindowsX.h>

#include "libopenmpt.hpp"
#include "libopenmpt_ext.hpp"

#include "resource.h"

#include <algorithm>
#include <locale>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <memory>

// settings

// {E45A45FF-66C5-4C8B-98F4-454046CF9468}
static const GUID guid_cfg_samplerate =
{ 0xe45a45ff, 0x66c5, 0x4c8b,{ 0x98, 0xf4, 0x45, 0x40, 0x46, 0xcf, 0x94, 0x68 } };

// {8289ADD7-E763-47A2-A1EF-40FE692C2F42}
static const GUID guid_cfg_channels =
{ 0x8289add7, 0xe763, 0x47a2,{ 0xa1, 0xef, 0x40, 0xfe, 0x69, 0x2c, 0x2f, 0x42 } };

// {5EA18C36-2D4C-4079-AEF1-08F6820C359E}
static const GUID guid_cfg_gain =
{ 0x5ea18c36, 0x2d4c, 0x4079,{ 0xae, 0xf1, 0x8, 0xf6, 0x82, 0xc, 0x35, 0x9e } };

// {B99BE38D-6547-42CB-B8A2-BD121224C9DB}
static const GUID guid_cfg_stereo =
{ 0xb99be38d, 0x6547, 0x42cb,{ 0xb8, 0xa2, 0xbd, 0x12, 0x12, 0x24, 0xc9, 0xdb } };

// {BB1D4C1D-81D3-46B0-9E9C-67E749BF6567}
static const GUID guid_cfg_repeat =
{ 0xbb1d4c1d, 0x81d3, 0x46b0,{ 0x9e, 0x9c, 0x67, 0xe7, 0x49, 0xbf, 0x65, 0x67 } };

// {39BD8352-47C0-4B1A-B2BA-9BE061602E26}
static const GUID guid_cfg_filter =
{ 0x39bd8352, 0x47c0, 0x4b1a,{ 0xb2, 0xba, 0x9b, 0xe0, 0x61, 0x60, 0x2e, 0x26 } };

// {8A887481-0F8B-49F8-95AF-AC392FCA5F21}
static const GUID guid_cfg_amiga =
{ 0x8a887481, 0xf8b, 0x49f8,{ 0x95, 0xaf, 0xac, 0x39, 0x2f, 0xca, 0x5f, 0x21 } };

// {EF0A23BA-316A-4991-B79B-264CADB23872}
static const GUID guid_cfg_ramping =
{ 0xef0a23ba, 0x316a, 0x4991,{ 0xb7, 0x9b, 0x26, 0x4c, 0xad, 0xb2, 0x38, 0x72 } };

// {CB7754E9-F36C-4270-9E7A-7A12251590CB}
static const GUID guid_cfg_history_samplerate =
{ 0xcb7754e9, 0xf36c, 0x4270,{ 0x9e, 0x7a, 0x7a, 0x12, 0x25, 0x15, 0x90, 0xcb } };

// {36FCD588-A661-4ED9-94B5-5C7AE662A27E}
static const GUID guid_cfg_length =
{ 0x36fcd588, 0xa661, 0x4ed9,{ 0x94, 0xb5, 0x5c, 0x7a, 0xe6, 0x62, 0xa2, 0x7e } };

// {5B8F75B0-6343-4F02-808D-71DF821A5EAE}
static const GUID guid_cfg_history_length =
{ 0x5b8f75b0, 0x6343, 0x4f02,{ 0x80, 0x8d, 0x71, 0xdf, 0x82, 0x1a, 0x5e, 0xae } };

// {E4836CC0-17FB-433C-A18F-45D954201F92}
static const GUID guid_ui_element =
{ 0xe4836cc0, 0x17fb, 0x433c,{ 0xa1, 0x8f, 0x45, 0xd9, 0x54, 0x20, 0x1f, 0x92 } };

static critical_section vis_lock;
static critical_section dlg_lock;

class monitor_dialog *     dialog = 0;

typedef std::shared_ptr<openmpt::module_ext> openmpt_handle;

static openmpt_handle current_mod;
static std::string current_mod_path;
static t_uint32 current_mod_subsong;

static openmpt_handle dlg_module;
static openmpt::ext::interactive * dlg_interactive = 0;

bool                       dlg_changed_info = false;
pfc::string8               dlg_path;
t_uint64                   dlg_channels_allowed = 0;

bool                       dlg_changed_controls = false;
t_uint64                   dlg_mute_mask = 0;
int                        dlg_pitch = 100;
int                        dlg_tempo = 100;

static const GUID guid_cfg_dlg_placement = { 0xbe06c7e5, 0x911b, 0x4341,{ 0xbb, 0x44, 0x8, 0x8, 0x7a, 0x6f, 0x41, 0xb7 } };
static cfg_window_placement cfg_dlg_placement(guid_cfg_dlg_placement);

static const GUID guid_cfg_control_override = { 0x22da836e, 0xc5e, 0x4741,{ 0x93, 0x71, 0x14, 0x24, 0x6f, 0xf9, 0xf6, 0xca } };
static cfg_int cfg_control_override(guid_cfg_control_override, 0);

static void monitor_start(openmpt_handle & mod, const char * p_path, bool playback);
static void monitor_update(openmpt_handle & mod);
static void monitor_stop(const openmpt_handle & mod);

enum {
	default_cfg_samplerate = 44100,
	default_cfg_channels = 2,
	default_cfg_gain = 0,
	default_cfg_stereo = 100,
	default_cfg_repeat = 0,
	default_cfg_filter = 8,
	default_cfg_amiga = 1,
	default_cfg_ramping = -1,
	default_cfg_length = 10
};

static cfg_int cfg_samplerate(guid_cfg_samplerate, default_cfg_samplerate);
static cfg_int cfg_channels(guid_cfg_channels, default_cfg_channels);
static cfg_int cfg_gain(guid_cfg_gain, default_cfg_gain);
static cfg_int cfg_stereo(guid_cfg_stereo, default_cfg_stereo);
static cfg_int cfg_repeat(guid_cfg_repeat, default_cfg_repeat);
static cfg_int cfg_filter(guid_cfg_filter, default_cfg_filter);
static cfg_int cfg_amiga(guid_cfg_amiga, default_cfg_amiga);
static cfg_int cfg_ramping(guid_cfg_ramping, default_cfg_ramping);
static cfg_int cfg_length(guid_cfg_length, default_cfg_length);

struct foo_openmpt_settings {
	int samplerate;
	int channels;
	int mastergain_millibel;
	int stereoseparation;
	int repeatcount;
	int interpolationfilterlength;
	int ramping;
	bool use_amiga_resampler;
	int minimum_length;
	foo_openmpt_settings() {

		/*
		samplerate = 48000;
		channels = 2;
		mastergain_millibel = 0;
		stereoseparation = 100;
		repeatcount = 0;
		interpolationfilterlength = 8;
		use_amiga_resampler = false;
		ramping = -1;
		*/

		pfc::string8 tmp;
		
		samplerate = cfg_samplerate;
		
		channels = cfg_channels;

		mastergain_millibel = cfg_gain;
		
		stereoseparation = cfg_stereo;

		repeatcount = cfg_repeat;
		
		interpolationfilterlength = cfg_filter;

		use_amiga_resampler = !!cfg_amiga;

		ramping = cfg_ramping;

		minimum_length = cfg_length;
	}
};

static void g_push_archive_extensions(std::vector<std::string> & list) {
    static std::string archive_extensions[] = {
        "mdz", "mdr", "s3z", "xmz", "itz", "mptmz"
    };
    for (unsigned i = 0, j = tabsize(archive_extensions); i < j; ++i) {
        if (list.empty() || std::find(list.begin(), list.end(), archive_extensions[i]) == list.end())
            list.push_back(archive_extensions[i]);
    }
}

static const char field_patterns[] = "mod_patterns";
static const char field_orders[] = "mod_orders";
static const char field_channels[] = "mod_channels";
static const char field_samples[] = "mod_samples";
static const char field_instruments[] = "mod_instruments";

static const char field_sample[] = "smpl";
static const char field_instrument[] = "inst";
static const char field_pattern[] = "patt";
static const char field_channel[] = "chan";

static const char field_dyn_order[] = "mod_dyn_order";
static const char field_dyn_pattern[] = "mod_dyn_pattern";
static const char field_dyn_row[] = "mod_dyn_row";
static const char field_dyn_speed[] = "mod_dyn_speed";
static const char field_dyn_tempo[] = "mod_dyn_tempo";
static const char field_dyn_channels[] = "mod_dyn_channels";
static const char field_dyn_channels_max[] = "mod_dyn_channels_max";

class input_openmpt : public input_stubs {
public:
	void open(service_ptr_t<file> p_filehint,const char * p_path,t_input_open_reason p_reason,abort_callback & p_abort) {
		if ( p_reason == input_open_info_write ) {
			throw exception_io_unsupported_format(); // our input does not support retagging.
		}
		m_path = p_path;
		m_file = p_filehint; // p_filehint may be null, hence next line
		input_open_file_helper(m_file,p_path,p_reason,p_abort); // if m_file is null, opens file with appropriate privileges for our operation (read/write for writing tags, read-only otherwise).

        try {
            service_ptr_t<file> m_unpack;
            unpacker::g_open( m_unpack, m_file, p_abort );
            
            m_file = m_unpack;
        }
        catch ( const exception_io_data & ) {
            m_file->seek( 0, p_abort );
        }

		if ( m_file->get_size( p_abort ) >= (std::numeric_limits<std::size_t>::max)() ) {
			throw exception_io_unsupported_format();
		}
		std::vector<char> data( static_cast<std::size_t>( m_file->get_size( p_abort ) ) );
		m_file->read( data.data(), data.size(), p_abort );
		try {
			std::map< std::string, std::string > ctls;
			ctls["seek.sync_samples"] = "1";
			mod = std::make_shared<openmpt::module_ext>( data, std::clog, ctls );
            for (unsigned i = 0, j = mod->get_num_subsongs(); i < j; ++i) {
                mod->select_subsong(i);
                lengths.push_back(mod->get_duration_seconds());
            }
            names = mod->get_subsong_names();
		} catch ( std::exception & /*e*/ ) {
			throw exception_io_data();
		}
		settings = foo_openmpt_settings();
		mod->set_repeat_count( settings.repeatcount );
		mod->set_render_param( openmpt::module::RENDER_MASTERGAIN_MILLIBEL, settings.mastergain_millibel );
		mod->set_render_param( openmpt::module::RENDER_STEREOSEPARATION_PERCENT, settings.stereoseparation );
		mod->set_render_param( openmpt::module::RENDER_INTERPOLATIONFILTER_LENGTH, settings.interpolationfilterlength );
		mod->set_render_param( openmpt::module::RENDER_VOLUMERAMPING_STRENGTH, settings.ramping );
		mod->ctl_set( "render.resampler.emulate_amiga", settings.use_amiga_resampler ? "1" : "0" );
		minimum_length = (double)settings.minimum_length;
	}
    void get_info(t_uint32 p_subsong,file_info & p_info,abort_callback & p_abort) {
		p_info.set_length( lengths[p_subsong] );
		p_info.info_set_int( "bitspersample", 32 );
		p_info.info_set_int(field_patterns, mod->get_num_patterns());
		p_info.info_set_int(field_orders, mod->get_num_orders());
		p_info.info_set_int(field_channels, mod->get_num_channels());
		p_info.info_set_int(field_samples, mod->get_num_samples());
		p_info.info_set_int(field_instruments, mod->get_num_instruments());
		std::vector<std::string> keys = mod->get_metadata_keys();
		for ( std::vector<std::string>::iterator key = keys.begin(); key != keys.end(); ++key ) {
			if ( *key == "message_raw" ) {
				continue;
			}
			std::string value = mod->get_metadata( *key );
            if ( *key == "title" ) {
                if ( names.size() > p_subsong && !names[p_subsong].empty() ) {
					if (!value.empty())
						p_info.meta_set( "album", value.c_str() );
                    p_info.meta_set( "title", names[p_subsong].c_str() );
                    continue;
                }
            }
			if (!value.empty())
				p_info.meta_set( (*key).c_str(), value.c_str() );
		}
		int i = 0;
		std::vector<std::string> _names = mod->get_sample_names();
		pfc::string8_fast temp;
		for (std::vector<std::string>::iterator name = _names.begin(); name != _names.end(); ++name, ++i) {
			if (name->empty()) continue;
			temp = field_sample;
			temp += pfc::format_int(i, 2, 10);
			p_info.meta_set(temp, name->c_str());
		}
		i = 0;
		_names = mod->get_instrument_names();
		for (std::vector<std::string>::iterator name = _names.begin(); name != _names.end(); ++name, ++i) {
			if (name->empty()) continue;
			temp = field_instrument;
			temp += pfc::format_int(i, 2, 10);
			p_info.meta_set(temp, name->c_str());
		}
		i = 0;
		_names = mod->get_channel_names();
		for (std::vector<std::string>::iterator name = _names.begin(); name != _names.end(); ++name, ++i) {
			if (name->empty()) continue;
			temp = field_channel;
			temp += pfc::format_int(i, 2, 10);
			p_info.meta_set(temp, name->c_str());
		}
		i = 0;
		_names = mod->get_pattern_names();
		for (std::vector<std::string>::iterator name = _names.begin(); name != _names.end(); ++name, ++i) {
			if (name->empty()) continue;
			temp = field_pattern;
			temp += pfc::format_int(i, 2, 10);
			p_info.meta_set(temp, name->c_str());
		}
	}
	t_filestats get_file_stats(abort_callback & p_abort) {
		return m_file->get_stats(p_abort);
	}
    t_uint32 get_subsong_count() {
		int32_t num_subsongs = mod->get_num_subsongs();
		int32_t computed_subsongs = num_subsongs;
		this->computed_subsongs.clear();
		for (int32_t i = 0; i < num_subsongs; ++i) {
			if (lengths[i] < minimum_length)
				--computed_subsongs;
			else
				this->computed_subsongs.push_back(i);
		}
		return computed_subsongs;
    }
    t_uint32 get_subsong(t_uint32 p_index) {
        return computed_subsongs[p_index];
    }
	void decode_initialize(t_uint32 p_subsong,unsigned p_flags,abort_callback & p_abort) {
		m_file->reopen(p_abort); // equivalent to seek to zero, except it also works on nonseekable streams
        mod->select_subsong(p_subsong);
        if (p_flags & input_flag_no_looping) {
            mod->set_repeat_count(0);
        }
		dyn_order = -1; dyn_row = -1; dyn_speed = -1; dyn_tempo = -1;
		dyn_channels = -1; dyn_max_channels = 0; dyn_pattern = -1;
		dyn_meta_reported = false;
		if (p_flags & input_flag_playback) {
			insync(vis_lock);
			current_mod = mod;
			current_mod_path = m_path;
			current_mod_subsong = p_subsong;
		}

		monitor_start(mod, m_path.c_str(), !!(p_flags & input_flag_playback));
	}
	bool decode_run(audio_chunk & p_chunk,abort_callback & p_abort) {
		monitor_update(mod);
		last_count = 0;
		if ( settings.channels == 1 ) {

			std::size_t count = mod->read( settings.samplerate, buffersize, left.data() );
			if ( count == 0 ) {
				return false;
			}
			last_count = count;
			for ( std::size_t frame = 0; frame < count; frame++ ) {
				buffer[frame*1+0] = left[frame];
			}
			p_chunk.set_data_32( buffer.data(), count, settings.channels, settings.samplerate );
			return true;

		} else if ( settings.channels == 2 ) {

			std::size_t count = mod->read( settings.samplerate, buffersize, left.data(), right.data() );
			if ( count == 0 ) {
				return false;
			}
			last_count = count;
			for ( std::size_t frame = 0; frame < count; frame++ ) {
				buffer[frame*2+0] = left[frame];
				buffer[frame*2+1] = right[frame];
			}
			p_chunk.set_data_32( buffer.data(), count, settings.channels, settings.samplerate );
			return true;

		} else if ( settings.channels == 4 ) {

			std::size_t count = mod->read( settings.samplerate, buffersize, left.data(), right.data(), rear_left.data(), rear_right.data() );
			if ( count == 0 ) {
				return false;
			}
			last_count = count;
			for ( std::size_t frame = 0; frame < count; frame++ ) {
				buffer[frame*4+0] = left[frame];
				buffer[frame*4+1] = right[frame];
				buffer[frame*4+2] = rear_left[frame];
				buffer[frame*4+3] = rear_right[frame];
			}
			p_chunk.set_data_32( buffer.data(), count, settings.channels, settings.samplerate );
			return true;

		} else {
			return false;
		}

	}
	void decode_seek(double p_seconds,abort_callback & p_abort) {
		dyn_meta_reported = false;
		last_count = 0;
		mod->set_position_seconds( p_seconds );
	}
	bool decode_can_seek() {
		return true;
	}
	bool decode_get_dynamic_info(file_info & p_out, double & p_timestamp_delta) {
		bool ret = false;
		if (!dyn_meta_reported) {
			p_out.info_set_int("samplerate", settings.samplerate);
			p_out.info_set_int("channels", settings.channels);
			dyn_meta_reported = true;
			ret = true;
		}
		int temp = mod->get_current_order();
		if (dyn_order != temp) {
			dyn_order = temp;
			p_out.info_set_int(field_dyn_order, dyn_order);
			p_out.info_set_int(field_dyn_pattern, mod->get_current_pattern());
			ret = true;
		}
		temp = mod->get_current_row();
		if (dyn_row != temp) {
			dyn_row = temp;
			p_out.info_set_int(field_dyn_row, dyn_row);
			ret = true;
		}
		temp = mod->get_current_speed();
		if (dyn_speed != temp) {
			dyn_speed = temp;
			p_out.info_set_int(field_dyn_speed, dyn_speed);
			ret = true;
		}
		temp = mod->get_current_tempo();
		if (dyn_tempo != temp) {
			dyn_tempo = temp;
			p_out.info_set_int(field_dyn_tempo, dyn_tempo);
			ret = true;
		}
		temp = mod->get_current_playing_channels();
		if (temp != dyn_channels) {
			dyn_channels = temp;
			p_out.info_set_int(field_dyn_channels, dyn_channels);
			ret = true;
		}
		if (temp > dyn_max_channels) {
			dyn_max_channels = temp;
			p_out.info_set_int(field_dyn_channels_max, dyn_max_channels);
			ret = true;
		}

		if (ret) {
			p_timestamp_delta = -((double)(last_count) / (double)(settings.samplerate));
		}
		return ret;
	}
	bool decode_get_dynamic_info_track(file_info & p_out, double & p_timestamp_delta) { // deals with dynamic information such as track changes in live streams
		return false;
	}
	void decode_on_idle(abort_callback & p_abort) {
		m_file->on_idle( p_abort );
	}
	void retag_set_info(t_uint32 p_subsong,const file_info & p_info,abort_callback & p_abort) {
		throw exception_tagging_unsupported();
	}
    void retag_commit(abort_callback & p_abort) {
        throw exception_tagging_unsupported();
    }
	static bool g_is_our_content_type(const char * p_content_type) { // match against supported mime types here
		return false;
	}
	static bool g_is_our_path(const char * p_path,const char * p_extension) {
		if ( !p_extension ) {
			return false;
		}
		std::vector<std::string> extensions = openmpt::get_supported_extensions();
        g_push_archive_extensions(extensions);
		std::string ext = p_extension;
		std::transform( ext.begin(), ext.end(), ext.begin(), tolower );
		return std::find( extensions.begin(), extensions.end(), ext ) != extensions.end();
	}
private:
	service_ptr_t<file> m_file;
	static const std::size_t buffersize = 1024;
	foo_openmpt_settings settings;
	openmpt_handle mod;
	std::vector<float> left;
	std::vector<float> right;
	std::vector<float> rear_left;
	std::vector<float> rear_right;
	std::vector<float> buffer;
    std::vector<double> lengths;
    std::vector<std::string> names;
	std::vector<int32_t> computed_subsongs;
	double minimum_length;
	bool dyn_meta_reported;
	std::size_t last_count;
	int dyn_order, dyn_row, dyn_speed, dyn_tempo, dyn_channels, dyn_max_channels;
	int dyn_pattern;
	std::string m_path;
    
public:
	input_openmpt() : left(buffersize), right(buffersize), rear_left(buffersize), rear_right(buffersize), buffer(4*buffersize) {}
	~input_openmpt() { monitor_stop(mod); }
    
    static GUID g_get_guid() {
        return { 0x894b056d, 0x989d, 0x46f6,{ 0x99, 0x78, 0x4d, 0xe2, 0x9f, 0x68, 0x3, 0x9f } };
    }
    
    static const char * g_get_name() {
        return "OpenMPT Module Decoder (kode54 fork)";
    }

	static GUID g_get_preferences_guid() {
		return { 0xb506c766, 0x7a78, 0x414f,{ 0x9e, 0xf2, 0x9, 0xcb, 0xa8, 0xf7, 0x81, 0x26 } };
	}
};

class input_file_type_v2_impl_openmpt : public input_file_type_v2 {
public:
    input_file_type_v2_impl_openmpt() {
        extensions = openmpt::get_supported_extensions();
        g_push_archive_extensions(extensions);
    }
    unsigned get_count() { return (unsigned) extensions.size(); }
    bool is_associatable(unsigned idx) { return true; }
    void get_format_name(unsigned idx, pfc::string_base & out, bool isPlural) {
        out = "OpenMPT compatible module file";
        if (isPlural) out += "s";
    }
    void get_extensions(unsigned idx, pfc::string_base & out) {
        out = extensions[idx].c_str();
    }
    
private:
    std::vector<std::string> extensions;
};

static cfg_dropdown_history cfg_history_samplerate(guid_cfg_history_samplerate, 16);

static const int srate_tab[] = { 8000,11025,16000,22050,24000,32000,44100,48000,64000,88200,96000 };

static cfg_dropdown_history cfg_history_length(guid_cfg_history_length, 16);

class CMyPreferences : public CDialogImpl<CMyPreferences>, public preferences_page_instance {
public:
	//Constructor - invoked by preferences_page_impl helpers - don't do Create() in here, preferences_page_impl does this for us
	CMyPreferences(preferences_page_callback::ptr callback) : m_callback(callback) {}

	//Note that we don't bother doing anything regarding destruction of our class.
	//The host ensures that our dialog is destroyed first, then the last reference to our preferences_page_instance object is released, causing our object to be deleted.


	//dialog resource ID
	enum { IDD = IDD_MOD_CONFIG };
	// preferences_page_instance methods (not all of them - get_wnd() is supplied by preferences_page_impl helpers)
	t_uint32 get_state();
	void apply();
	void reset();

	//WTL message map
	BEGIN_MSG_MAP(CMyPreferences)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDC_SAMPLERATE, CBN_EDITCHANGE, OnEditChange)
		COMMAND_HANDLER_EX(IDC_LENGTH, CBN_EDITCHANGE, OnEditChange)
		COMMAND_HANDLER_EX(IDC_SAMPLERATE, CBN_SELCHANGE, OnSelectionChange)
		COMMAND_HANDLER_EX(IDC_LENGTH, CBN_SELCHANGE, OnSelectionChange)
		DROPDOWN_HISTORY_HANDLER(IDC_SAMPLERATE, cfg_history_samplerate)
		DROPDOWN_HISTORY_HANDLER(IDC_LENGTH, cfg_history_length)
		COMMAND_HANDLER_EX(IDC_CHANNELS, CBN_SELCHANGE, OnSelectionChange)
		COMMAND_HANDLER_EX(IDC_INTERPOLATION, CBN_SELCHANGE, OnSelectionChange)
		COMMAND_HANDLER_EX(IDC_LOOPS, CBN_SELCHANGE, OnSelectionChange)
		COMMAND_HANDLER_EX(IDC_AMIGA, BN_CLICKED, OnButtonClick)
		MSG_WM_HSCROLL(OnHScroll)
	END_MSG_MAP()
private:
	BOOL OnInitDialog(CWindow, LPARAM);
	void OnEditChange(UINT, int, CWindow);
	void OnSelectionChange(UINT, int, CWindow);
	void OnButtonClick(UINT, int, CWindow);
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar pScrollBar);

	bool HasChanged();
	void OnChanged();

	void update_label_gain();
	void update_label_stereo();
	void update_label_ramping();

	const preferences_page_callback::ptr m_callback;

	CComboBox m_combo_samplerate, m_combo_channels, m_combo_filter, m_combo_loops, m_combo_length;
	CTrackBarCtrl m_slider_gain, m_slider_stereo, m_slider_ramping;
	CCheckBox m_check_amiga;

	CStatic m_text_gain, m_text_stereo, m_text_ramping;
};

static const char filter_to_dialog[9] = { -1, 0, 1, -1, 2, -1, -1, -1, 3 };
static const char dialog_to_filter[4] = { 1, 2, 4, 8 };

static const char channels_to_dialog[5] = { -1, 0, 1, -1, 2 };
static const char dialog_to_channels[3] = { 1, 2, 4 };

BOOL CMyPreferences::OnInitDialog(CWindow, LPARAM) {
	char temp[16];
	int n;

	SetWindowLong(DWL_USER, 0);

	for (n = tabsize(srate_tab); n--;) {
		if (srate_tab[n] != cfg_samplerate) {
			_itoa(srate_tab[n], temp, 10);
			cfg_history_samplerate.add_item(temp);
		}
	}
	_itoa(cfg_samplerate, temp, 10);
	cfg_history_samplerate.add_item(temp);
	m_combo_samplerate = GetDlgItem(IDC_SAMPLERATE);
	cfg_history_samplerate.setup_dropdown(m_combo_samplerate);
	m_combo_samplerate.SetCurSel(0);

	for (n = 11; n--;) {
		if (n != cfg_length) {
			_itoa(n, temp, 10);
			cfg_history_length.add_item(temp);
		}
	}
	_itoa(cfg_length, temp, 10);
	cfg_history_length.add_item(temp);
	m_combo_length = GetDlgItem(IDC_LENGTH);
	cfg_history_length.setup_dropdown(m_combo_length);
	m_combo_length.SetCurSel(0);

	m_combo_channels = GetDlgItem(IDC_CHANNELS);
	uSendMessageText(m_combo_channels, CB_ADDSTRING, 0, "mono");
	uSendMessageText(m_combo_channels, CB_ADDSTRING, 0, "stereo");
	uSendMessageText(m_combo_channels, CB_ADDSTRING, 0, "surround");

	if (cfg_channels < 0 && cfg_channels > 4 && channels_to_dialog[cfg_channels] < 0)
		cfg_channels = default_cfg_channels;

	m_combo_channels.SetCurSel(channels_to_dialog[cfg_channels]);

	m_combo_filter = GetDlgItem(IDC_INTERPOLATION);
	uSendMessageText(m_combo_filter, CB_ADDSTRING, 0, "none");
	uSendMessageText(m_combo_filter, CB_ADDSTRING, 0, "linear");
	uSendMessageText(m_combo_filter, CB_ADDSTRING, 0, "cubic");
	uSendMessageText(m_combo_filter, CB_ADDSTRING, 0, "sinc");

	if (cfg_filter < 1 || cfg_filter > 8 || filter_to_dialog[cfg_filter] < 0)
		cfg_filter = default_cfg_filter;

	m_combo_filter.SetCurSel(filter_to_dialog[cfg_filter]);

	m_combo_loops = GetDlgItem(IDC_LOOPS);
	uSendMessageText(m_combo_loops, CB_ADDSTRING, 0, "infinite");
	uSendMessageText(m_combo_loops, CB_ADDSTRING, 0, "none");
	for (n = 1; n <= 16; n++) {
		_itoa(n, temp, 10);
		uSendMessageText(m_combo_loops, CB_ADDSTRING, 0, temp);
	}

	if (cfg_repeat < -1 || cfg_repeat > 16)
		cfg_repeat = default_cfg_repeat;

	m_combo_loops.SetCurSel(cfg_repeat + 1);

	m_check_amiga = GetDlgItem(IDC_AMIGA);
	m_check_amiga.SetCheck(cfg_amiga);

	m_slider_gain = GetDlgItem(IDC_GAIN);
	m_slider_gain.SetRangeMin(0);
	m_slider_gain.SetRangeMax(2400);

	if (cfg_gain < -1200 || cfg_gain > 1200)
		cfg_gain = default_cfg_gain;

	m_slider_gain.SetPos(cfg_gain + 1200);

	temp[15] = '\0';

	m_text_gain = GetDlgItem(IDC_GAIN_TEXT);
	update_label_gain();

	m_slider_stereo = GetDlgItem(IDC_STEREO);
	m_slider_stereo.SetRangeMin(0);
	m_slider_stereo.SetRangeMax(200);

	if (cfg_stereo < 0 || cfg_stereo > 200)
		cfg_stereo = default_cfg_stereo;

	m_slider_stereo.SetPos(cfg_stereo);

	m_text_stereo = GetDlgItem(IDC_STEREO_TEXT);
	update_label_stereo();

	m_slider_ramping = GetDlgItem(IDC_RAMPING);
	m_slider_ramping.SetRangeMin(0);
	m_slider_ramping.SetRangeMax(11);

	if (cfg_ramping < -1 || cfg_ramping > 10)
		cfg_ramping = default_cfg_ramping;

	m_slider_ramping.SetPos(cfg_ramping + 1);

	m_text_ramping = GetDlgItem(IDC_RAMPING_TEXT);
	update_label_ramping();

	SetWindowLong(DWL_USER, 1);

	return FALSE;
}

void CMyPreferences::update_label_gain() {
	char temp[16];
	int value = m_slider_gain.GetPos() - 1200;
	bool negative = value < 0;
	if (negative) value = -value;
	temp[15] = '\0';
	snprintf(temp, 15, "%c%d.%02d dB", negative ? '-' : '+', value / 100, value % 100);
	uSetWindowText(m_text_gain, temp);
}

void CMyPreferences::update_label_stereo() {
	char temp[16];
	int value = m_slider_stereo.GetPos();
	temp[15] = '\0';
	snprintf(temp, 15, "%d%%", value);
	uSetWindowText(m_text_stereo, temp);
}

void CMyPreferences::update_label_ramping() {
	char temp[16];
	int value = m_slider_ramping.GetPos() - 1;
	temp[15] = '\0';
	if (value == -1) strncpy(temp, "default", 15);
	else if (value == 0) strncpy(temp, "off", 15);
	else snprintf(temp, 15, "%d ms", value);
	uSetWindowText(m_text_ramping, temp);
}

void CMyPreferences::OnEditChange(UINT, int, CWindow) {
	if (GetWindowLong(DWL_USER)) {
		OnChanged();
	}
}

void CMyPreferences::OnSelectionChange(UINT, int, CWindow) {
	OnChanged();
}

void CMyPreferences::OnButtonClick(UINT, int, CWindow) {
	OnChanged();
}

void CMyPreferences::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar pScrollBar) {
	if (pScrollBar.m_hWnd == m_slider_gain.m_hWnd)
		update_label_gain();
	else if (pScrollBar.m_hWnd == m_slider_stereo.m_hWnd)
		update_label_stereo();
	else if (pScrollBar.m_hWnd == m_slider_ramping.m_hWnd)
		update_label_ramping();
	OnChanged();
}

t_uint32 CMyPreferences::get_state() {
	t_uint32 state = preferences_state::resettable;
	if (HasChanged()) state |= preferences_state::changed;
	return state;
}

void CMyPreferences::reset() {
	SetDlgItemInt(IDC_SAMPLERATE, default_cfg_samplerate, FALSE);
	SetDlgItemInt(IDC_LENGTH, default_cfg_length, FALSE);
	m_combo_channels.SetCurSel(channels_to_dialog[default_cfg_channels]);
	m_combo_filter.SetCurSel(filter_to_dialog[default_cfg_filter]);
	m_combo_loops.SetCurSel(default_cfg_repeat + 1);
	m_check_amiga.SetCheck(default_cfg_amiga);
	m_slider_gain.SetPos(default_cfg_gain + 1200);
	m_slider_stereo.SetPos(default_cfg_stereo);
	m_slider_ramping.SetPos(default_cfg_ramping + 1);

	update_label_gain();
	update_label_stereo();
	update_label_ramping();

	OnChanged();
}

void CMyPreferences::apply() {
	char temp[16];
	int t = GetDlgItemInt(IDC_SAMPLERATE, NULL, FALSE);
	if (t < 6000) t = 6000;
	else if (t > 96000) t = 96000;
	SetDlgItemInt(IDC_SAMPLERATE, t, FALSE);
	_itoa(t, temp, 10);
	cfg_history_samplerate.add_item(temp);
	cfg_samplerate = t;
	t = GetDlgItemInt(IDC_LENGTH, NULL, FALSE);
	if (t > 20) t = 20;
	SetDlgItemInt(IDC_LENGTH, t, FALSE);
	_itoa(t, temp, 10);
	cfg_history_length.add_item(temp);
	cfg_length = t;
	t = m_combo_channels.GetCurSel();
	if (t >= 0 && t < tabsize(dialog_to_channels))
		cfg_channels = dialog_to_channels[t];
	t = m_combo_filter.GetCurSel();
	if (t >= 0 && t < tabsize(dialog_to_filter))
		cfg_filter = dialog_to_filter[t];
	t = m_combo_loops.GetCurSel();
	if (t >= 0 && t <= 17)
		cfg_repeat = t - 1;
	cfg_amiga = m_check_amiga.GetCheck();
	t = m_slider_gain.GetPos();
	if (t >= 0 && t <= 2400)
		cfg_gain = t - 1200;
	t = m_slider_stereo.GetPos();
	if (t >= 0 && t <= 200)
		cfg_stereo = t;
	t = m_slider_ramping.GetPos();
	if (t >= 0 && t <= 11)
		cfg_ramping = t - 1;

	OnChanged(); //our dialog content has not changed but the flags have - our currently shown values now match the settings so the apply button can be disabled
}

bool CMyPreferences::HasChanged() {
	//returns whether our dialog content is different from the current configuration (whether the apply button should be enabled or not)
	bool changed = false;
	int t;
	if (!changed && GetDlgItemInt(IDC_SAMPLERATE, NULL, FALSE) != cfg_samplerate) changed = true;
	if (!changed && GetDlgItemInt(IDC_LENGTH, NULL, FALSE) != cfg_length) changed = true;
	if (!changed) {
		t = m_combo_channels.GetCurSel();
		if (t >= 0 && t < tabsize(dialog_to_channels) && dialog_to_channels[t] != cfg_channels)
			changed = true;
	}
	if (!changed) {
		t = m_combo_filter.GetCurSel();
		if (t >= 0 && t < tabsize(dialog_to_filter) && dialog_to_filter[t] != cfg_filter)
			changed = true;
	}
	if (!changed && (m_combo_loops.GetCurSel() - 1) != cfg_repeat) changed = true;
	if (!changed && m_check_amiga.GetCheck() != cfg_amiga) changed = true;
	if (!changed && (m_slider_gain.GetPos() - 1200) != cfg_gain) changed = true;
	if (!changed && m_slider_stereo.GetPos() != cfg_stereo) changed = true;
	if (!changed && (m_slider_ramping.GetPos() - 1) != cfg_ramping) changed = true;
	return changed;
}
void CMyPreferences::OnChanged() {
	//tell the host that our state has changed to enable/disable the apply button appropriately.
	m_callback->on_state_changed();
}

class preferences_page_myimpl : public preferences_page_impl<CMyPreferences> {
	// preferences_page_impl<> helper deals with instantiation of our dialog; inherits from preferences_page_v3.
public:
	const char * get_name() { return input_openmpt::g_get_name(); }
	GUID get_guid() { return input_openmpt::g_get_preferences_guid(); }
	GUID get_parent_guid() { return guid_input; }
};

enum ColorIndex
{
	col_background = 0,
	col_unknown,
	col_text,
	col_empty,
	col_instr,
	col_vol,
	col_pitch,
	col_global,

	col_max
};

static ColorIndex effect_type_to_color_index(openmpt::ext::pattern_vis::effect_type effect_type) {
	switch (effect_type) {
	case openmpt::ext::pattern_vis::effect_unknown: return col_unknown; break;
	case openmpt::ext::pattern_vis::effect_general: return col_text; break;
	case openmpt::ext::pattern_vis::effect_global: return col_global; break;
	case openmpt::ext::pattern_vis::effect_volume: return col_vol; break;
	case openmpt::ext::pattern_vis::effect_panning: return col_instr; break;
	case openmpt::ext::pattern_vis::effect_pitch: return col_pitch; break;
	default: return col_unknown; break;
	}
}

const struct Columns
{
	int num_chars;
	int color;
} pattern_columns[] = {
	{ 3, col_text },	// C-5
{ 2, col_instr },	// 01
{ 3, col_vol },		// v64
{ 3, col_pitch },	// EFF
};

static const int max_cols = 4;

static void assure_width(std::string & str, std::size_t width) {
	if (str.length() == width) {
		return;
	}
	else if (str.length() < width) {
		str += std::string(width - str.length(), ' ');
	}
	else if (str.length() > width) {
		str = str.substr(0, width);
	}
}

union Color
{
	struct { uint8_t r, g, b, a; };
	COLORREF dw;
};

class CVisWindow : public CWindowImpl<CVisWindow>, play_callback {
	HDC visDC;
	HGDIOBJ visbitmap;

	Color viscolors[col_max];
	HPEN vispens[col_max];
	HBRUSH visbrushs[col_max];
	HFONT visfont;
	int last_pattern;
	int last_row;

	int current_pattern;
	int current_row;

	std::string path;
	t_uint32 subsong;

	bool running;

	openmpt_handle mod;
	openmpt::ext::pattern_vis * pattern_vis;

	SIZE m_hSize;
	HDC m_hDC, m_hDCbackbuffer;
	HBITMAP m_hBitbackbuffer;

protected:
	HFONT deffont;
	DWORD colors[3];

public:
	CVisWindow() {
		running = false;
		current_pattern = -1;
		current_row = -1;

		{
			insync(vis_lock);
			mod = current_mod;
			if (mod) {
				pattern_vis = static_cast<openmpt::ext::pattern_vis *>(mod->get_interface(openmpt::ext::pattern_vis_id));
				running = true;
				last_pattern = -1;
			}
		}
	}

	DECLARE_WND_CLASS_EX(_T("8D2F5E34-CFB2-4187-AA5D-AD067A9BF07B"), 0, -1)

	BEGIN_MSG_MAP(CVisWindow)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_SIZE(OnSize)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()

private:
	BOOL VisOpen();
	void VisClose();
	BOOL VisRenderDC(HDC dc, SIZE size);

	void FB2KAPI on_playback_starting(play_control::t_track_command, bool) {}
	void FB2KAPI on_playback_edited(metadb_handle_ptr) {}
	void FB2KAPI on_playback_seek(double) {}
	void FB2KAPI on_playback_pause(bool p_state) {}
	void FB2KAPI on_playback_dynamic_info_track(const file_info &) {}
	void FB2KAPI on_playback_time(double) {}
	void FB2KAPI on_volume_change(float) {}

	void FB2KAPI on_playback_new_track(metadb_handle_ptr p_track);
	void FB2KAPI on_playback_stop(play_control::t_stop_reason);
	void FB2KAPI on_playback_dynamic_info(const file_info & p_info);

protected:
	int OnCreate(LPCREATESTRUCT);
	void OnDestroy();
	void OnPaint(CDCHandle dc);
	void OnSize(UINT wParam, CSize size);

	void Render();
};

void CVisWindow::on_playback_new_track(metadb_handle_ptr p_track) {
	if (p_track.is_valid()) {
		insync(vis_lock);
		path = p_track->get_path();
		subsong = p_track->get_subsong_index();
		if (current_mod && path == current_mod_path && subsong == current_mod_subsong) {
			mod = current_mod;
			pattern_vis = static_cast<openmpt::ext::pattern_vis *>(mod->get_interface(openmpt::ext::pattern_vis_id));
			running = true;
			last_pattern = -1;
			return;
		}
	}
	current_pattern = -1;
	running = false;
	last_pattern = -1;
	Invalidate();
}

void CVisWindow::on_playback_stop(play_control::t_stop_reason) {
	insync(vis_lock);
	current_pattern = -1;
	pattern_vis = 0;
	running = false;
	last_pattern = -1;
	Invalidate();
}

void CVisWindow::on_playback_dynamic_info(const file_info & p_info)
{
	const char * p_pattern = p_info.info_get(field_dyn_pattern);
	const char * p_row = p_info.info_get(field_dyn_row);
	if (running && p_pattern && p_row) {
		char * end;
		unsigned long temp = strtoul(p_pattern, &end, 10);
		if (temp >= 0 && end > p_pattern)
			current_pattern = (int)temp;

		temp = strtoul(p_row, &end, 10);
		if (temp >= 0 && end > p_row)
			current_row = (int)temp;

		if (current_pattern != last_pattern ||
			current_row != last_row)
			Invalidate();
	}
}

static Color invert_color(Color c) {
	Color res;
	res.a = c.a;
	res.r = 255 - c.r;
	res.g = 255 - c.g;
	res.b = 255 - c.b;
	return res;
}

int CVisWindow::OnCreate(LPCREATESTRUCT)
{
	SetWindowText(_T("Module Visualizer"));

	m_hDC = GetDC();
	m_hDCbackbuffer = CreateCompatibleDC(m_hDC);
	m_hBitbackbuffer = 0;

	VisOpen();

	last_pattern = -1;

	Invalidate();

	static_api_ptr_t<play_callback_manager>()->register_callback(this, play_callback::flag_on_playback_new_track | play_callback::flag_on_playback_stop | play_callback::flag_on_playback_dynamic_info, false);

	CenterWindow();

	return TRUE;
}

void CVisWindow::OnDestroy()
{
	running = false;
	last_pattern = -1;
	Render();

	static_api_ptr_t<play_callback_manager>()->unregister_callback(this);

	VisClose();

	if (m_hBitbackbuffer) {
		DeleteObject(m_hBitbackbuffer);
		m_hBitbackbuffer = 0;
	}

	DeleteDC(m_hDCbackbuffer);
	DeleteObject(deffont);
}

void CVisWindow::OnPaint(CDCHandle dc) {
	Render();

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(&ps);

	ps.rcPaint.right -= ps.rcPaint.left;
	ps.rcPaint.bottom -= ps.rcPaint.top;

	BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top,
		ps.rcPaint.right, ps.rcPaint.bottom, m_hDCbackbuffer,
		ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

	EndPaint(&ps);
}

void CVisWindow::Render() {
	RECT rc;
	GetClientRect(&rc);

	SIZE sz;
	sz.cx = rc.right - rc.left;
	sz.cy = rc.bottom - rc.top;

	if (sz.cx != m_hSize.cx || sz.cy != m_hSize.cy || !m_hBitbackbuffer) {
		if (m_hBitbackbuffer) {
			DeleteObject(m_hBitbackbuffer);
			m_hBitbackbuffer = 0;
		}

		m_hSize.cx = sz.cx;
		m_hSize.cy = sz.cy;

		BITMAPINFOHEADER bmih;
		bmih.biSize = sizeof(bmih);
		bmih.biWidth = sz.cx;
		bmih.biHeight = sz.cy;
		bmih.biPlanes = 1;
		bmih.biBitCount = 32;
		bmih.biCompression = BI_RGB;
		bmih.biSizeImage = 0;
		bmih.biXPelsPerMeter = 0;
		bmih.biYPelsPerMeter = 0;
		bmih.biClrUsed = 0;
		bmih.biClrImportant = 0;

		m_hBitbackbuffer = CreateDIBSection(0, (const BITMAPINFO *)&bmih, DIB_RGB_COLORS, 0, 0, 0);
		SelectObject(m_hDCbackbuffer, m_hBitbackbuffer);
	}

	VisRenderDC(m_hDCbackbuffer, sz);
}

void CVisWindow::OnSize(UINT wParam, CSize size) {
	last_pattern = -1;
	Invalidate();
}

BOOL CVisWindow::VisOpen() {
	insync(vis_lock);
	visDC = 0;
	visbitmap = 0;
	visfont = 0;

	viscolors[col_background].dw = colors[0];
	viscolors[col_unknown].dw = colors[1];
	viscolors[col_text].dw = colors[2];

	viscolors[col_global] = invert_color(viscolors[col_background]);

	const int r = viscolors[col_text].r, g = viscolors[col_text].g, b = viscolors[col_text].b;
	viscolors[col_empty].r = (r + viscolors[col_background].r) / 2;
	viscolors[col_empty].g = (g + viscolors[col_background].g) / 2;
	viscolors[col_empty].b = (b + viscolors[col_background].b) / 2;
	viscolors[col_empty].a = 0;

#define MIXCOLOR(col, c1, c2, c3) { \
	viscolors[col] = viscolors[col_text]; \
	int mix = viscolors[col].c1 + 0xA0; \
	viscolors[col].c1 = mix; \
	if ( mix > 0xFF ) { \
		viscolors[col].c2 = std::max<uint8_t>( c2 - viscolors[col].c1 / 2, 0 ); \
		viscolors[col].c3 = std::max<uint8_t>( c3 - viscolors[col].c1 / 2, 0 ); \
		viscolors[col].c1 = 0xFF; \
	} }

	MIXCOLOR(col_instr, g, r, b);
	MIXCOLOR(col_vol, b, r, g);
	MIXCOLOR(col_pitch, r, g, b);
#undef MIXCOLOR

	for (int i = 0; i < col_max; ++i) {
		vispens[i] = CreatePen(PS_SOLID, 1, viscolors[i].dw);
		visbrushs[i] = CreateSolidBrush(viscolors[i].dw);
	}

	if (!mod) {
		return FALSE;
	}

	return TRUE;
}

void CVisWindow::VisClose() {
	insync(vis_lock);

	for (int i = 0; i < col_max; ++i) {
		DeletePen(vispens[i]);
		DeleteBrush(visbrushs[i]);
	}

	DeleteFont(visfont);
	DeleteBitmap(visbitmap);
	DeleteDC(visDC);
}

static int get_pattern_width(int chars_per_channel, int spaces_per_channel, int num_cols, int text_size, int channels) {
	int pattern_width = ((chars_per_channel * channels + 4) * text_size) + (spaces_per_channel * channels + channels - (num_cols == 1 ? 1 : 2)) * (text_size / 2);
	return pattern_width;
}

BOOL CVisWindow::VisRenderDC(HDC dc, SIZE size) {
	if (!mod || current_pattern < 0) {
		RECT rc;
		rc.left = 0; rc.right = size.cx;
		rc.top = 0; rc.bottom = size.cy;
		FillRect(dc, &rc, visbrushs[col_background]);
		return FALSE;
	}

	insync(vis_lock);
	RECT rect;

	if (!visfont) {
		// Force usage of a nice monospace font
		LOGFONT logfont;
		GetObject(deffont, sizeof(logfont), &logfont);
		wcscpy(logfont.lfFaceName, L"Lucida Console");
		visfont = CreateFontIndirect(&logfont);
	}
	SIZE text_size;
	SelectFont(dc, visfont);
	if (GetTextExtentPoint32(dc, TEXT("W"), 1, &text_size) == FALSE) {
		return FALSE;
	}

	int pattern = current_pattern;
	int _current_row = current_row;

	const std::size_t channels = mod->get_num_channels();
	const std::size_t rows = mod->get_pattern_num_rows(pattern);

	const std::size_t num_half_chars = std::max<std::size_t>(2 * size.cx / text_size.cx, 8) - 8;
	const std::size_t num_rows = size.cy / text_size.cy;

	// Spaces between pattern components are half width, full space at channel end
	const std::size_t half_chars_per_channel = num_half_chars / channels;
	std::size_t chars_per_channel, spaces_per_channel;
	std::size_t num_cols;
	std::size_t col0_width = pattern_columns[0].num_chars;
	for (num_cols = sizeof(pattern_columns) / sizeof(pattern_columns[0]); num_cols >= 1; num_cols--) {
		chars_per_channel = 0;
		spaces_per_channel = num_cols > 1 ? num_cols : 0;	// No extra space if we only display notes
		for (std::size_t i = 0; i < num_cols; i++) {
			chars_per_channel += pattern_columns[i].num_chars;
		}

		if (half_chars_per_channel >= chars_per_channel * 2 + spaces_per_channel + 1 || num_cols == 1) {
			break;
		}
	}

	int pattern_width = get_pattern_width(chars_per_channel, spaces_per_channel, num_cols, text_size.cx, channels);
	int pattern_height = rows * text_size.cy;

	if (!visDC || last_pattern != pattern) {
		DeleteBitmap(visbitmap);
		DeleteDC(visDC);

		visDC = CreateCompatibleDC(dc);
		visbitmap = CreateCompatibleBitmap(dc, pattern_width, pattern_height);
		SelectBitmap(visDC, visbitmap);

		SelectBrush(visDC, vispens[col_unknown]);
		SelectBrush(visDC, visbrushs[col_background]);

		SelectFont(visDC, visfont);

		rect.top = 0;
		rect.left = 0;
		rect.right = pattern_width;
		rect.bottom = pattern_height;
		FillRect(visDC, &rect, visbrushs[col_background]);

		SetBkColor(visDC, viscolors[col_background].dw);

		POINT pos;
		pos.y = 0;

		for (std::size_t row = 0; row < rows; row++) {
			pos.x = 0;

			std::ostringstream s;
			s.imbue(std::locale::classic());
			s << std::setfill('0') << std::setw(3) << row;
			const std::string rowstr = s.str();

			SetTextColor(visDC, viscolors[1].dw);
			TextOutA(visDC, pos.x, pos.y, rowstr.c_str(), rowstr.length());
			pos.x += 4 * text_size.cx;

			for (std::size_t channel = 0; channel < channels; ++channel) {

				struct coldata {
					std::string text;
					bool is_empty;
					ColorIndex color;
					coldata()
						: is_empty(false)
						, color(col_unknown)
					{
						return;
					}
				};

				coldata cols[max_cols];

				for (std::size_t col = 0; col < max_cols; ++col) {
					switch (col) {
					case 0:
						cols[col].text = mod->format_pattern_row_channel_command(pattern, row, channel, openmpt::module::command_note);
						break;
					case 1:
						cols[col].text = mod->format_pattern_row_channel_command(pattern, row, channel, openmpt::module::command_instrument);
						break;
					case 2:
						cols[col].text = mod->format_pattern_row_channel_command(pattern, row, channel, openmpt::module::command_volumeffect)
							+ mod->format_pattern_row_channel_command(pattern, row, channel, openmpt::module::command_volume);
						break;
					case 3:
						cols[col].text = mod->format_pattern_row_channel_command(pattern, row, channel, openmpt::module::command_effect)
							+ mod->format_pattern_row_channel_command(pattern, row, channel, openmpt::module::command_parameter);
						break;
					}
					int color = pattern_columns[col].color;
					if (pattern_vis && (col == 2 || col == 3)) {
						if (col == 2) {
							color = effect_type_to_color_index(pattern_vis->get_pattern_row_channel_volume_effect_type(pattern, row, channel));
						}
						if (col == 3) {
							color = effect_type_to_color_index(pattern_vis->get_pattern_row_channel_effect_type(pattern, row, channel));
						}
					}
					switch (cols[col].text[0]) {
					case ' ':
					case '.':
						cols[col].is_empty = true;
					case '^':
					case '=':
					case '~':
						color = col_empty;
						break;
					}
					cols[col].color = (ColorIndex)color;

				}

				if (num_cols <= 3 && !cols[3].is_empty) {
					if (cols[2].is_empty) {
						cols[2] = cols[3];
					}
					else if (cols[0].is_empty) {
						cols[0] = cols[3];
					}
				}

				if (num_cols <= 2 && !cols[2].is_empty) {
					if (cols[0].is_empty) {
						cols[0] = cols[2];
					}
				}

				for (std::size_t col = 0; col < num_cols; ++col) {

					std::size_t col_width = (num_cols > 1) ? pattern_columns[col].num_chars : col0_width;

					assure_width(cols[col].text, col_width);

					SetTextColor(visDC, viscolors[cols[col].color].dw);
					TextOutA(visDC, pos.x, pos.y, cols[col].text.c_str(), cols[col].text.length());
					pos.x += col_width * text_size.cx + text_size.cx / 2;
				}
				// Extra padding
				if (num_cols > 1) {
					pos.x += text_size.cx / 2;
				}
			}

			pos.y += text_size.cy;
		}
	}

	rect.top = 0;
	rect.left = 0;
	rect.right = size.cx;
	rect.bottom = size.cy;
	FillRect(dc, &rect, visbrushs[col_background]);

	int offset_x = (size.cx - pattern_width) / 2;
	int offset_y = (size.cy - text_size.cy) / 2 - _current_row * text_size.cy;
	int src_offset_x = 0;
	int src_offset_y = 0;

	if (offset_x < 0) {
		src_offset_x -= offset_x;
		pattern_width = std::min<int>(pattern_width + offset_x, size.cx);
		offset_x = 0;
	}

	if (offset_y < 0) {
		src_offset_y -= offset_y;
		pattern_height = std::min<int>(pattern_height + offset_y, size.cy);
		offset_y = 0;
	}

	BitBlt(dc, offset_x, offset_y, pattern_width, pattern_height, visDC, src_offset_x, src_offset_y, SRCCOPY);

	// Highlight current row
	rect.left = offset_x;
	rect.top = (size.cy - text_size.cy) / 2;
	rect.right = rect.left + pattern_width;
	rect.bottom = rect.top + text_size.cy;
	InvertRect(dc, &rect);

	last_pattern = pattern;

	return TRUE;
}

class CVisWindowElementConfig : public ui_element_config {
public:
	struct data {
	} the_data;

	virtual GUID get_guid() const { return guid_ui_element; }
	virtual const void * get_data() const { return &the_data; }
	virtual t_size get_data_size() const { return sizeof(the_data); }
};

class CVisWindowElementInstance;

critical_section g_VisWindows_sync;
pfc::ptr_list_t<CVisWindowElementInstance> g_VisWindows;

class CVisWindowElementInstance : public CVisWindow, public ui_element_instance {
	ui_element_instance_callback_ptr m_callback;

public:
	CVisWindowElementInstance(HWND hwndParent, ui_element_config::ptr cfg, ui_element_instance_callback_ptr p_callback) : CVisWindow() {
		m_callback = p_callback;

		t_ui_color color;

		if (!m_callback->query_color(ui_color_background, color)) {
			color = GetSysColor(ui_color_to_sys_color_index(ui_color_background));
		}
		colors[0] = color;
		if (!m_callback->query_color(ui_color_selection, color)) {
			color = GetSysColor(ui_color_to_sys_color_index(ui_color_selection));
		}
		colors[1] = color;
		if (!m_callback->query_color(ui_color_text, color)) {
			color = GetSysColor(ui_color_to_sys_color_index(ui_color_text));
		}
		colors[2] = color;

		deffont = m_callback->query_font_ex(ui_font_default);

		Create(hwndParent, 0, 0, 0, 0, 0U, 0);

		{
			insync(g_VisWindows_sync);
			g_VisWindows.add_item(this);
		}
	}

	~CVisWindowElementInstance() {
		{
			insync(g_VisWindows_sync);
			g_VisWindows.remove_item(this);
		}
		if (m_hWnd) DestroyWindow();
	}

	void UpdateLayout() {
		Invalidate();
	}

	virtual HWND get_wnd() { return m_hWnd; }

	virtual void set_configuration(ui_element_config::ptr data) {
		if (data->get_guid() == get_guid() && data->get_data_size() == 0) {
			UpdateLayout();
		}
	}

	virtual ui_element_config::ptr get_configuration() {
		CVisWindowElementConfig * config = new service_impl_t<CVisWindowElementConfig>;
		return config;
	}

	virtual GUID get_guid() { return guid_ui_element; }
	virtual GUID get_subclass() { return guid_ui_element; }

	virtual ui_element_min_max_info get_min_max_info() {
		ui_element_min_max_info ret;
		ret.m_min_width = 32;
		ret.m_max_width = 1024 * 1024;
		ret.m_min_height = 8;
		ret.m_max_height = 1024 * 1024;
		return ret;
	}

	virtual bool edit_mode_context_menu_test(const POINT & p_point, bool p_fromkeyboard) { return false; }
	virtual void edit_mode_context_menu_build(const POINT & p_point, bool p_fromkeyboard, HMENU p_menu, unsigned p_id_base) {
	}
	virtual void edit_mode_context_menu_command(const POINT & p_point, bool p_fromkeyboard, unsigned p_id, unsigned p_id_base) {
	}
	virtual bool edit_mode_context_menu_get_focus_point(POINT & p_point) {
		return false;
	}
	virtual bool edit_mode_context_menu_get_description(unsigned p_id, unsigned p_id_base, pfc::string_base & p_out) {
		return false;
	}

	void Activate() {
		m_callback->request_activation(this);
	}
};

class CVisWindowElement : public ui_element_v2 {
	virtual GUID get_guid() { return guid_ui_element; }
	virtual GUID get_subclass() { return ui_element_subclass_playback_visualisation; }
	virtual void get_name(pfc::string_base & p_out) { p_out = "OpenMPT"; }
	virtual ui_element_instance_ptr instantiate(HWND p_parent, ui_element_config::ptr cfg, ui_element_instance_callback_ptr p_callback) {
		return new window_service_impl_t<CVisWindowElementInstance>(p_parent, cfg, p_callback);
	}
	virtual ui_element_config::ptr get_default_configuration() { return new service_impl_t<CVisWindowElementConfig>; }
	virtual ui_element_children_enumerator_ptr enumerate_children(ui_element_config::ptr cfg) { return NULL; }
	virtual bool get_description(pfc::string_base & p_out) { p_out = "Displays the current pattern in the module which is currently playing."; return true; }
	virtual t_uint32 get_flags() { return KFlagSupportsBump | KFlagHavePopupCommand; }
	virtual bool bump() {
		insync(g_VisWindows_sync);
		if (g_VisWindows.get_count()) {
			for (unsigned i = 0, j = g_VisWindows.get_count(); i < j; i++)
				g_VisWindows[i]->Activate();
			return true;
		}
		else return false;
	}
};

static void initialize_channels() {
	dlg_channels_allowed = 0;

	if (dlg_module) {
		int n_channels = dlg_module->get_num_channels();
		for (int order = 0, n_orders = dlg_module->get_num_orders(); order < n_orders; ++order) {
			int pattern = dlg_module->get_order_pattern(order);
			if (pattern < dlg_module->get_num_patterns()) {
				for (int row = 0, n_rows = dlg_module->get_pattern_num_rows(pattern); row < n_rows; ++row) {
					for (int channel = 0; channel < n_channels; ++channel) {
						int note = dlg_module->get_pattern_row_channel_command(pattern, row, channel, openmpt::module::command_note);
						if (note) {
							dlg_channels_allowed |= t_uint64(1) << channel;
						}
					}
				}
			}
		}
	}
}

static void mute_channels(openmpt_handle & mod, openmpt::ext::interactive *interactive, t_uint64 mask) {
	if (interactive) {
		for (int channel = 0, n_channels = mod->get_num_channels(); channel < n_channels; ++channel) {
			t_uint64 current_mask = t_uint64(1) << channel;
			bool muted = !!(mask & current_mask);
			interactive->set_channel_mute_status(channel, muted);
		}
	}
}

static void adjust_speed(openmpt::ext::interactive *interactive, int pitch, int tempo) {
	if (interactive) {
		interactive->set_pitch_factor(double(pitch) * 0.01);
		interactive->set_tempo_factor(double(tempo) * 0.01);
	}
}

void monitor_start(openmpt_handle & mod, const char * p_path, bool playback) {
	insync(dlg_lock);

	openmpt::ext::interactive *interactive = static_cast<openmpt::ext::interactive *>(mod->get_interface(openmpt::ext::interactive_id));

	if (playback) {
		dlg_changed_info = true;

		dlg_module = mod;
		dlg_interactive = interactive;
		dlg_path = p_path;

		initialize_channels();
	}

	if (cfg_control_override) {
		mute_channels(mod, interactive, dlg_mute_mask);
		adjust_speed(interactive, dlg_pitch, dlg_tempo);
	}
}

void monitor_update(openmpt_handle & mod)
{
	insync(dlg_lock);

	if (mod == dlg_module) {
		if (dlg_changed_controls) {
			dlg_changed_controls = false;

			bool enabled = !!cfg_control_override;
			t_uint64 mask = enabled ? dlg_mute_mask : 0;
			int pitch = enabled ? dlg_pitch : 100;
			int tempo = enabled ? dlg_tempo : 100;

			mute_channels(dlg_module, dlg_interactive, mask);
			adjust_speed(dlg_interactive, pitch, tempo);
		}
	}
}

void monitor_stop(const openmpt_handle & mod)
{
	insync(dlg_lock);

	if (mod == dlg_module) {
		dlg_module.reset();
		dlg_interactive = 0;

		dlg_changed_info = true;
		dlg_path = "";

		dlg_channels_allowed = 0;
	}
}

class monitor_dialog {
	HWND wnd;
	HWND wnd_slider_pitch;
	HWND wnd_slider_tempo;

	static BOOL CALLBACK g_dialog_proc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
		monitor_dialog * ptr;

		if (msg == WM_INITDIALOG) {
			ptr = reinterpret_cast<monitor_dialog *> (lp);
			uSetWindowLong(wnd, DWL_USER, lp);
		}
		else {
			ptr = reinterpret_cast<monitor_dialog *> (uGetWindowLong(wnd, DWL_USER));
		}

		if (ptr) return ptr->dialog_proc(wnd, msg, wp, lp);
		else return 0;
	}

	BOOL dialog_proc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
		switch (msg) {
		case WM_INITDIALOG: {
			this->wnd = wnd;

			uSendDlgItemMessage(wnd, IDC_OVERRIDE, BM_SETCHECK, cfg_control_override, 0);

			HWND w = wnd_slider_pitch = GetDlgItem(wnd, IDC_PITCH);
			SendMessage(w, TBM_SETRANGEMIN, FALSE, 1);
			SendMessage(w, TBM_SETRANGEMAX, FALSE, 400);

			w = wnd_slider_tempo = GetDlgItem(wnd, IDC_TEMPO);
			SendMessage(w, TBM_SETRANGEMIN, FALSE, 1);
			SendMessage(w, TBM_SETRANGEMAX, FALSE, 400);

			{
				insync(dlg_lock);
				dlg_changed_info = false;
				dlg_changed_controls = false;
				update();
			}

			SetTimer(wnd, 0, 100, 0);

			cfg_dlg_placement.on_window_creation(wnd);
		}
		return 1;

		case WM_TIMER: {
			insync(dlg_lock);
			if (dlg_changed_info) {
				dlg_changed_info = false;
				update();
			}
		}
		break;

		case WM_DESTROY: {
			cfg_dlg_placement.on_window_destruction(wnd);
			KillTimer(wnd, 0);
			uSetWindowLong(wnd, DWL_USER, 0);
			delete this;
			dialog = 0;
		}
		break;

		case WM_HSCROLL: {
			HWND wndctrl = (HWND)lp;
			char temp[8] = { 0 };

			if (wndctrl == wnd_slider_pitch) {
				insync(dlg_lock);
				dlg_pitch = SendMessage(wndctrl, TBM_GETPOS, 0, 0);
				dlg_changed_controls = true;
				snprintf(temp, 7, "%d%%", dlg_pitch);
				uSetDlgItemText(wnd, IDC_PITCH_TEXT, temp);
			}
			else if (wndctrl == wnd_slider_tempo) {
				insync(dlg_lock);
				dlg_tempo = SendMessage(wndctrl, TBM_GETPOS, 0, 0);
				dlg_changed_controls = true;
				snprintf(temp, 7, "%d%%", dlg_tempo);
				uSetDlgItemText(wnd, IDC_TEMPO_TEXT, temp);
			}
			break;
		}

		case WM_COMMAND:
			if (wp == IDCANCEL) {
				DestroyWindow(wnd);
			}
			else if (wp == IDC_OVERRIDE) {
				insync(dlg_lock);

				cfg_control_override = uSendMessage((HWND)lp, BM_GETCHECK, 0, 0);

				BOOL enable = !!dlg_module && cfg_control_override;

				for (unsigned i = 0, j = 64; i < j; ++i) {
					EnableWindow(GetDlgItem(wnd, IDC_VOICE1 + i), enable);
				}

				EnableWindow(wnd_slider_pitch, enable);
				EnableWindow(wnd_slider_tempo, enable);

				dlg_changed_controls = true;
			}
			else if (wp == IDC_RESET) {
				insync(dlg_lock);

				dlg_changed_controls = dlg_mute_mask != 0 || dlg_pitch != 100 || dlg_tempo != 100;
				dlg_mute_mask = 0;
				dlg_pitch = 100;
				dlg_tempo = 100;

				if (dlg_changed_controls) {
					update();
				}
			}
			else if (wp - IDC_VOICE1 < 64) {
				unsigned voice = wp - IDC_VOICE1;
				t_uint64 mask = ~(1 << voice);
				t_uint64 bit = uSendMessage((HWND)lp, BM_GETCHECK, 0, 0) ? 0 : (1 << voice);

				insync(dlg_lock);

				dlg_changed_controls = true;
				dlg_mute_mask = (dlg_mute_mask & mask) | bit;
			}
			break;
		}

		return 0;
	}

	void update() {
		pfc::string8 title;
		if (dlg_path.length()) {
			title = pfc::string_filename_ext(dlg_path);
			title += " - ";
		}
		title += "OpenMPT";
		uSetWindowText(wnd, title);

		BOOL enable = !!dlg_module && cfg_control_override;

		HWND w;
		for (unsigned i = 0; i < 64; ++i) {
			w = GetDlgItem(wnd, IDC_VOICE1 + i);
			uSendMessage(w, BM_SETCHECK, !((dlg_mute_mask >> i) & 1), 0);
			EnableWindow(w, enable);
			ShowWindow(w, ((t_uint64(1) << i) & dlg_channels_allowed) ? SW_SHOWNA : SW_HIDE);
		}

		EnableWindow(wnd_slider_pitch, enable);
		EnableWindow(wnd_slider_tempo, enable);

		SendMessage(wnd_slider_pitch, TBM_SETPOS, TRUE, dlg_pitch);
		SendMessage(wnd_slider_tempo, TBM_SETPOS, TRUE, dlg_tempo);

		char temp[8];

		snprintf(temp, 7, "%d%%", dlg_pitch);
		uSetDlgItemText(wnd, IDC_PITCH_TEXT, temp);

		snprintf(temp, 7, "%d%%", dlg_tempo);
		uSetDlgItemText(wnd, IDC_TEMPO_TEXT, temp);
	}

public:
	monitor_dialog(HWND parent) {
		wnd = 0;
		if (!CreateDialogParam(core_api::get_my_instance(), MAKEINTRESOURCE(IDD_MONITOR), parent, g_dialog_proc, reinterpret_cast<LPARAM> (this))) {
			throw exception_win32(GetLastError());
		}
	}

	~monitor_dialog() {
		DestroyWindow(wnd);
	}
};

class monitor_menu : public mainmenu_commands {
	virtual t_uint32 get_command_count() {
		return 1;
	}

	virtual GUID get_command(t_uint32 p_index) {
		// {5766C0F0-1933-4E21-BE3A-0842258C9CE2}
		static const GUID guid =
		{ 0x5766c0f0, 0x1933, 0x4e21,{ 0xbe, 0x3a, 0x8, 0x42, 0x25, 0x8c, 0x9c, 0xe2 } };
		return guid;
	}

	virtual void get_name(t_uint32 p_index, pfc::string_base & p_out) {
		p_out = "OpenMPT control";
	}

	virtual bool get_description(t_uint32 p_index, pfc::string_base & p_out) {
		p_out = "Activates the OpenMPT advanced controls window.";
		return true;
	}

	virtual GUID get_parent() {
		return mainmenu_groups::view;
	}

	virtual bool get_display(t_uint32 p_index, pfc::string_base & p_text, t_uint32 & p_flags) {
		p_flags = 0;
		get_name(p_index, p_text);
		return true;
	}

	virtual void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
		if (p_index == 0 && core_api::assert_main_thread()) {
			if (!dialog) {
				try {
					dialog = new monitor_dialog(core_api::get_main_window());
				}
				catch (const std::exception & e) {
					dialog = 0;
					console::error(e.what());
				}
			}
		}
	}
};

static input_factory_t<input_openmpt> g_input_openmpt_factory;
static preferences_page_factory_t<preferences_page_myimpl> g_config_openmpt_factory;
static service_factory_single_t<input_file_type_v2_impl_openmpt> g_filetypes;
static service_factory_single_t<CVisWindowElement> g_element_openmpt_vis_factory;
static mainmenu_commands_factory_t<monitor_menu> g_mainmenu_commands_monitor_factory;

DECLARE_COMPONENT_VERSION("OpenMPT component (kode54 fork)", OPENMPT_API_VERSION_STRING BUILD_VERSION, "libopenmpt based module file player\n\nForked by kode54.");

VALIDATE_COMPONENT_FILENAME("foo_openmpt54.dll");
