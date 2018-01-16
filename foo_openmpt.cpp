//#define BUILD_VERSION ""
#define BUILD_VERSION "+3"

#if defined(_MSC_VER)
#pragma warning(disable:4091)
#endif

#include <foobar2000.h>
#include "../helpers/dropdown_helper.h"
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


// Declaration of your component's version information
// Since foobar2000 v1.0 having at least one of these in your DLL is mandatory to let the troubleshooter tell different versions of your component apart.
// Note that it is possible to declare multiple components within one DLL, but it's strongly recommended to keep only one declaration per DLL.
// As for 1.1, the version numbers are used by the component update finder to find updates; for that to work, you must have ONLY ONE declaration per DLL. If there are multiple declarations, the component is assumed to be outdated and a version number of "0" is assumed, to overwrite the component with whatever is currently on the site assuming that it comes with proper version numbers.
DECLARE_COMPONENT_VERSION("OpenMPT component", OPENMPT_API_VERSION_STRING BUILD_VERSION ,"libopenmpt based module file player");

// This will prevent users from renaming your component around (important for proper troubleshooter behaviors) or loading multiple instances of it.
VALIDATE_COMPONENT_FILENAME("foo_openmpt.dll");


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

// {E4836CC0-17FB-433C-A18F-45D954201F92}
static const GUID guid_ui_element =
{ 0xe4836cc0, 0x17fb, 0x433c,{ 0xa1, 0x8f, 0x45, 0xd9, 0x54, 0x20, 0x1f, 0x92 } };


enum {
	default_cfg_samplerate = 44100,
	default_cfg_channels = 2,
	default_cfg_gain = 0,
	default_cfg_stereo = 100,
	default_cfg_repeat = 0,
	default_cfg_filter = 8,
	default_cfg_amiga = 1,
	default_cfg_ramping = -1
};

static cfg_int cfg_samplerate(guid_cfg_samplerate, default_cfg_samplerate);
static cfg_int cfg_channels(guid_cfg_channels, default_cfg_channels);
static cfg_int cfg_gain(guid_cfg_gain, default_cfg_gain);
static cfg_int cfg_stereo(guid_cfg_stereo, default_cfg_stereo);
static cfg_int cfg_repeat(guid_cfg_repeat, default_cfg_repeat);
static cfg_int cfg_filter(guid_cfg_filter, default_cfg_filter);
static cfg_int cfg_amiga(guid_cfg_amiga, default_cfg_amiga);
static cfg_int cfg_ramping(guid_cfg_ramping, default_cfg_ramping);

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
		
		samplerate = cfg_samplerate;
		
		channels = cfg_channels;

		mastergain_millibel = cfg_gain;
		
		stereoseparation = cfg_stereo;

		repeatcount = cfg_repeat;
		
		interpolationfilterlength = cfg_filter;

		use_amiga_resampler = !!cfg_amiga;

		ramping = cfg_ramping;
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

class openmpt_handle {
	int refcount;
	openmpt::module_ext * mod;

public:
	openmpt_handle() : mod(0), refcount(0) { }
	~openmpt_handle() {
		delete mod;
	}

	openmpt::module_ext * operator * () { return mod; }

	void assign(openmpt::module_ext * _mod) {
		if (_mod == mod)
			++refcount;
		else {
			if (refcount == 1)
				delete mod;
			mod = _mod;
			refcount = 1;
		}
	}
	bool release(openmpt::module_ext * _mod) {
		if (_mod == mod) {
			if (--refcount == 0) {
				delete mod;
				mod = 0;
			}
			return true;
		}
		return false;
	}
};

static critical_section vis_lock;
static openmpt_handle current_mod;
static std::string current_mod_path;
static t_uint32 current_mod_subsong;

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
			mod = new openmpt::module_ext( data, std::clog, ctls );
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
		if (p_flags & input_flag_playback) {
			insync(vis_lock);
			current_mod.assign(mod);
			current_mod_path = m_path;
			current_mod_subsong = p_subsong;
		}
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
	openmpt::module_ext * mod;
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
	std::string m_path;
    
public:
	input_openmpt() : mod(0), left(buffersize), right(buffersize), rear_left(buffersize), rear_right(buffersize), buffer(4*buffersize) {}
	~input_openmpt() {
		insync(vis_lock);
		if (!current_mod.release(mod))
			delete mod;
	}
    
    static GUID g_get_guid() {
        return { 0xe2ff4f22, 0xb217, 0x46e2, { 0xb2, 0xf4, 0xa8, 0xbd, 0x9f, 0x9a, 0x71, 0xe6 } };
    }
    
    static const char * g_get_name() {
        return "OpenMPT Module Decoder";
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
		COMMAND_HANDLER_EX(IDC_SAMPLERATE, CBN_SELCHANGE, OnSelectionChange)
		DROPDOWN_HISTORY_HANDLER(IDC_SAMPLERATE, cfg_history_samplerate)
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

	CComboBox m_combo_samplerate, m_combo_channels, m_combo_filter, m_combo_loops;
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

	openmpt::module_ext * mod;
	openmpt::ext::pattern_vis * pattern_vis;

	SIZE m_hSize;
	HDC m_hDC, m_hDCbackbuffer;
	HBITMAP m_hBitbackbuffer;

protected:
	DWORD colors[3];

public:
	CVisWindow() {
		running = false;
		current_pattern = -1;
		current_row = -1;

		{
			insync(vis_lock);
			mod = *current_mod;
			if (mod) {
				current_mod.assign(mod);
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
		if (*current_mod && path == current_mod_path && subsong == current_mod_subsong) {
			if (mod)
				delete mod;
			mod = *current_mod;
			if (mod) {
				current_mod.assign(mod);
				pattern_vis = static_cast<openmpt::ext::pattern_vis *>(mod->get_interface(openmpt::ext::pattern_vis_id));
			}
			running = true;
			last_pattern = -1;
			return;
		}
	}
	running = false;
	last_pattern = -1;
	Invalidate();
}

void CVisWindow::on_playback_stop(play_control::t_stop_reason) {
	insync(vis_lock);
	current_mod.release(mod);
	mod = 0;
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

	if (mod) {
		current_mod.release(mod);
		mod = 0;
	}
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
		GetObject(GetCurrentObject(dc, OBJ_FONT), sizeof(logfont), &logfont);
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

static input_factory_t<input_openmpt> g_input_openmpt_factory;
static preferences_page_factory_t<preferences_page_myimpl> g_config_openmpt_factory;
static service_factory_single_t<input_file_type_v2_impl_openmpt> g_filetypes;
static service_factory_single_t<CVisWindowElement> g_element_openmpt_vis_factory;
