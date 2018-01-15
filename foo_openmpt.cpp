
#if defined(_MSC_VER)
#pragma warning(disable:4091)
#endif

#include "foobar2000/SDK/foobar2000.h"

#include "libopenmpt.hpp"

#include <algorithm>
#include <locale>
#include <string>
#include <vector>



// Declaration of your component's version information
// Since foobar2000 v1.0 having at least one of these in your DLL is mandatory to let the troubleshooter tell different versions of your component apart.
// Note that it is possible to declare multiple components within one DLL, but it's strongly recommended to keep only one declaration per DLL.
// As for 1.1, the version numbers are used by the component update finder to find updates; for that to work, you must have ONLY ONE declaration per DLL. If there are multiple declarations, the component is assumed to be outdated and a version number of "0" is assumed, to overwrite the component with whatever is currently on the site assuming that it comes with proper version numbers.
DECLARE_COMPONENT_VERSION("OpenMPT component", OPENMPT_API_VERSION_STRING "-" __TIMESTAMP__,"libopenmpt based module file player");

// This will prevent users from renaming your component around (important for proper troubleshooter behaviors) or loading multiple instances of it.
VALIDATE_COMPONENT_FILENAME("foo_openmpt.dll");


// settings

// {FFD659CA-6AEA-479f-8E60-F03B297286FE}
static const GUID guid_openmpt_root = 
{ 0xffd659ca, 0x6aea, 0x479f, { 0x8e, 0x60, 0xf0, 0x3b, 0x29, 0x72, 0x86, 0xfe } };


// {E698E101-FD93-4e6c-AF02-AEC7E8631D8E}
static const GUID guid_openmpt_samplerate = 
{ 0xe698e101, 0xfd93, 0x4e6c, { 0xaf, 0x2, 0xae, 0xc7, 0xe8, 0x63, 0x1d, 0x8e } };

// {E4026686-02F9-4805-A3FD-2EECA655A92C}
static const GUID guid_openmpt_channels = 
{ 0xe4026686, 0x2f9, 0x4805, { 0xa3, 0xfd, 0x2e, 0xec, 0xa6, 0x55, 0xa9, 0x2c } };

// {7C845F81-9BA3-4a9a-9C94-C7056DFD1B57}
static const GUID guid_openmpt_gain = 
{ 0x7c845f81, 0x9ba3, 0x4a9a, { 0x9c, 0x94, 0xc7, 0x5, 0x6d, 0xfd, 0x1b, 0x57 } };

// {EDB0E1E5-BD2E-475c-B2FB-8448C92F6F29}
static const GUID guid_openmpt_stereo = 
{ 0xedb0e1e5, 0xbd2e, 0x475c, { 0xb2, 0xfb, 0x84, 0x48, 0xc9, 0x2f, 0x6f, 0x29 } };

// {9115A820-67F5-4d0a-B0FB-D312F7FBBAFF}
static const GUID guid_openmpt_repeat = 
{ 0x9115a820, 0x67f5, 0x4d0a, { 0xb0, 0xfb, 0xd3, 0x12, 0xf7, 0xfb, 0xba, 0xff } };

// {EAAD5E60-F75B-4071-B308-9956362ECB69}
static const GUID guid_openmpt_filter = 
{ 0xeaad5e60, 0xf75b, 0x4071, { 0xb3, 0x8, 0x99, 0x56, 0x36, 0x2e, 0xcb, 0x69 } };

// {0CF7E156-44E3-4587-A727-864B045BEDDD}
static const GUID guid_openmpt_amiga =
{ 0x0cf7e156, 0x44e3, 0x4587,{ 0xa7, 027, 0x86, 0x4b, 0x04, 0x5b, 0xed, 0xdd } };

// {497BF503-D825-4A02-BE5C-02DB8911B1DC}
static const GUID guid_openmpt_ramping = 
{ 0x497bf503, 0xd825, 0x4a02, { 0xbe, 0x5c, 0x2, 0xdb, 0x89, 0x11, 0xb1, 0xdc } };


static advconfig_branch_factory g_advconfigBranch("OpenMPT Component", guid_openmpt_root, advconfig_branch::guid_branch_decoding, 0);

static advconfig_integer_factory   cfg_samplerate("Samplerate [6000..96000] (Hz)"                                     , guid_openmpt_samplerate, guid_openmpt_root, 0, 48000, 6000, 96000);
static advconfig_integer_factory   cfg_channels  ("Channels [1=mono, 2=stereo, 4=quad]"                               , guid_openmpt_channels  , guid_openmpt_root, 0,     2,    1,     4);
static advconfig_string_factory_MT cfg_gain      ("Gain [-12...12] (dB)"                                              , guid_openmpt_gain      , guid_openmpt_root, 0, "0.0");
static advconfig_integer_factory   cfg_stereo    ("Stereo separation [0...200] (%)"                                   , guid_openmpt_stereo    , guid_openmpt_root, 0,   100,    0,   200);
static advconfig_string_factory_MT cfg_repeat    ("Repeat [0=never, -1=forever, 1..] (#)"                             , guid_openmpt_repeat    , guid_openmpt_root, 0,   "0");
static advconfig_integer_factory   cfg_filter    ("Interpolation filter length [1=nearest, 2=linear, 4=cubic, 8=sinc]", guid_openmpt_filter    , guid_openmpt_root, 0,     8,    1,     8);
static advconfig_checkbox_factory  cfg_amiga     ("Use Amiga Resampler for Amiga modules"                             , guid_openmpt_amiga,      guid_openmpt_root, 0, false);
static advconfig_string_factory_MT cfg_ramping   ("Volume ramping [-1=default, 0=off, 1..10] (ms)"                    , guid_openmpt_ramping   , guid_openmpt_root, 0,  "-1");

struct foo_openmpt_settings {
	int samplerate;
	int channels;
	int mastergain_millibel;
	int stereoseparation;
	int repeatcount;
	int interpolationfilterlength;
	int ramping;
	bool use_amiga_resampler;
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
		
		samplerate = static_cast<int>( cfg_samplerate.get() );
		
		channels = static_cast<int>( cfg_channels.get() );
		if ( channels == 3 ) {
			channels = 2;
		}
		
		cfg_gain.get(tmp);
		mastergain_millibel = static_cast<int>( pfc::string_to_float( tmp ) * 100.0 );
		
		stereoseparation = static_cast<int>( cfg_stereo );

		cfg_repeat.get(tmp);
		repeatcount = static_cast<int>( pfc::atoi64_ex( tmp, ~0 ) );
		if ( repeatcount < -1 ) {
			repeatcount = 0;
		}
		
		interpolationfilterlength = static_cast<int>( cfg_filter.get() );
		if (interpolationfilterlength == 0) {
			interpolationfilterlength = 1;
		}

		use_amiga_resampler = cfg_amiga.get();
		
		cfg_ramping.get(tmp);
		ramping = static_cast<int>( pfc::atoi64_ex( tmp, ~0 ) );
		if ( ramping < -1 ) {
			ramping = -1;
		}

	}
};



// Sample initquit implementation. See also: initquit class documentation in relevant header.

#if 0
class myinitquit : public initquit {
public:
	void on_init() {
		// console::print("Sample component: on_init()");
	}
	void on_quit() {
		// console::print("Sample component: on_quit()");
	}
};

static initquit_factory_t<myinitquit> g_myinitquit_factory;
#endif

static void g_push_archive_extensions(std::vector<std::string> & list)
{
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
		m_file = p_filehint; // p_filehint may be null, hence next line
		input_open_file_helper(m_file,p_path,p_reason,p_abort); // if m_file is null, opens file with appropriate privileges for our operation (read/write for writing tags, read-only otherwise).

        try
        {
            service_ptr_t<file> m_unpack;
            unpacker::g_open( m_unpack, m_file, p_abort );
            
            m_file = m_unpack;
        }
        catch ( const exception_io_data & )
        {
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
			mod = new openmpt::module( data, std::clog, ctls );
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
            if ( *key == "title" ) {
                if ( names.size() > p_subsong && names[p_subsong].length() ) {
                    p_info.meta_set( "album", mod->get_metadata( *key ).c_str() );
                    p_info.meta_set( "title", names[p_subsong].c_str() );
                    continue;
                }
            }
			p_info.meta_set( (*key).c_str(), mod->get_metadata( *key ).c_str() );
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
        return mod->get_num_subsongs();
    }
    t_uint32 get_subsong(t_uint32 p_index) {
        return p_index;
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
	}
	bool decode_run(audio_chunk & p_chunk,abort_callback & p_abort) {
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
	openmpt::module * mod;
	std::vector<float> left;
	std::vector<float> right;
	std::vector<float> rear_left;
	std::vector<float> rear_right;
	std::vector<float> buffer;
    std::vector<double> lengths;
    std::vector<std::string> names;
	bool dyn_meta_reported;
	std::size_t last_count;
	int dyn_order, dyn_row, dyn_speed, dyn_tempo, dyn_channels, dyn_max_channels;
	int dyn_pattern;
    
public:
	input_openmpt() : mod(0), left(buffersize), right(buffersize), rear_left(buffersize), rear_right(buffersize), buffer(4*buffersize) {}
	~input_openmpt() { delete mod; mod = 0; }
    
    static GUID g_get_guid()
    {
        return { 0xe2ff4f22, 0xb217, 0x46e2, { 0xb2, 0xf4, 0xa8, 0xbd, 0x9f, 0x9a, 0x71, 0xe6 } };
    }
    
    static const char * g_get_name()
    {
        return "OpenMPT Module Decoder";
    }
};

static input_factory_t<input_openmpt> g_input_openmpt_factory;

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

static service_factory_single_t<input_file_type_v2_impl_openmpt> g_filetypes;