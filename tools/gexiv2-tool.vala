/*
 * gexiv2-tool.vala
 *
 * Author(s)
 *  Jim Nelson <jim@yorba.org>
 *
 * This is free software. See COPYING for details.
 */

// Global variables used in parsing program parameters
bool print_exif_tags = false;
bool print_iptc_tags = false;
bool print_xmp_tags = false;
bool print_xmp_ns = false;
bool print_tag_details = false;
string print_single_tag;
bool print_unknown_tags = false;

// Command line optional parameters
const GLib.OptionEntry[] options = {
    // [-exif]
    {"exif", 'e', 0, OptionArg.NONE, ref print_exif_tags, "Print out Exif tags from FILE", null},

    // [-iptc]
    {"iptc", 'i', 0, OptionArg.NONE, ref print_iptc_tags, "Print out Iptc tags from FILE", null},

    // [-xmp]
    {"xmp", 'x', 0, OptionArg.NONE, ref print_xmp_tags, "Print out Xmp tags from FILE", null},

    // [-xns]
    {"xns", 'n', 0, OptionArg.NONE, ref print_xmp_ns, "Print out Xmp namespaces from FILE", null},

    // [-details]
    {"details", 'd', 0, OptionArg.NONE, ref print_tag_details, "Print additional tag details (interpreted value, label, type and description)", null},

    // [-tag TAG]
    {"tag", 't', 0, OptionArg.STRING, ref print_single_tag, "Only print a specific tag's data from FILE (overrides -e, -i and -x)", null},

    // [-u]
    {"tag", 'u', 0, OptionArg.NONE, ref print_unknown_tags, "Include unknown tags when printing (e.g. Exif.Sony2Fp.0x018f)", null},

    // list terminator
    {null}
};

/**
 * main:
 * @args: Program arguments
 *
 * Prints out image metadata from a file.
 *
 * The supported image formats can be found at <ulink url="https://www.exiv2.org/manpage.html"></ulink>
 * and the Exiv2 Tag Reference at <ulink url="https://www.exiv2.org/metadata.html"></ulink>.
 *
 * Returns: 0 for success or error code
 */
int main(string[] args) {

    var program_name = extract_filename(args[0]);
    print_single_tag = "";

    GExiv2.initialize();

    // Setup and parse program parameters
    OptionContext opt_context;
    try {
        string summary = "";
        summary = summary + "Prints out metadata in an image file. If no optional parameters are specified,\n";
        summary = summary + "then the default is print all tags (\"-e -i -x\"). The order of parameters is\n";
        summary = summary + "not respected.\n";

        string desc = "";
        desc = desc + "Image metadata is stored in 3 types: Exif, Iptc and Xmp and a file may contain\n";
        desc = desc + "more than one type. Each tag is in the format 'familyName.groupName.tagName',\n";
        desc = desc + "e.g. 'Exif.Image.DateTime'. Each unique Xmp groupName has a namespace, e.g.\n";
        desc = desc + "The standard registration for 'dc' is 'http://purl.org/dc/elements/1.1/'\n";
        desc = desc + " \n";
        desc = desc + "Supported image formats can be found at https://www.exiv2.org/manpage.html and\n";
        desc = desc + "Exiv2 metadata tag references at https://www.exiv2.org/metadata.html\n";
        desc = desc + " \n";
        desc = desc + "Examples:\n";
        desc = desc + "  " + program_name + " FILE                         - Print all the tags in FILE (\"-e -i -x\")\n";
        desc = desc + "  " + program_name + " -u FILE                      - Print all the tags (including unknown ones) in FILE (\"-u -d -e -i -x\")\n";
        desc = desc + "  " + program_name + " -d FILE                      - Print all the tag details in FILE (\"-d -e -i -x\")\n";
        desc = desc + "  " + program_name + " -x -n FILE                   - Print the Xmp namespaces and Xmp tag values in FILE\n";
        desc = desc + "  " + program_name + " -d -e -i FILE                - Print the Exif and Iptc tag details in FILE\n";
        desc = desc + "  " + program_name + " -t Xmp.dc.subject -d -e FILE - Print only the 'Xmp.dc.subject' tag details in FILE\n";

        opt_context = new OptionContext("FILE");
        opt_context.set_description(desc);
        opt_context.set_summary(summary);
        opt_context.set_help_enabled(true);
        opt_context.add_main_entries(options, null);
        opt_context.parse(ref args);

        // Only 1 required parameter allowed
        if (args.length < 2) {
            printerr("%s error: no FILE provided\n", program_name);
            print(opt_context.get_help(true, null));
            return 1;
        }
    
        if (args.length > 2) {
            printerr("%s error: Invalid parameters\n", program_name);
            print(opt_context.get_help(true, null));
            return 1;
        }

        // If no user selected tag type options then print them all
        if (!print_exif_tags && !print_iptc_tags && !print_xmp_tags) {
            print_exif_tags = true;
            print_iptc_tags = true;
            print_xmp_tags = true;
        }

        // Try to open the user's file
        GExiv2.Metadata metadata = new GExiv2.Metadata();
        metadata.open_path(args[1]);

        // Get tag names from file
        var exif_tags = metadata.get_exif_tags();
        var iptc_tags = metadata.get_iptc_tags();
        var xmp_tags = metadata.get_xmp_tags();

        // Print user selected options
        if (print_single_tag != "") {
            if (print_single_tag in exif_tags || print_single_tag in iptc_tags || print_single_tag in xmp_tags) {
                if (print_xmp_ns) {
                    // Print namespace for the specific tag only
                    string[] tags = {print_single_tag};
                    print_all_xmp_namespaces(tags);
                }
                print_tag(metadata, print_single_tag, print_unknown_tags, print_tag_details);
            } else {
                string error_message = "Tag `" + print_single_tag + "' not found in metadata";
                throw new GLib.Error(Quark.from_string(program_name), 1, error_message);
            }
        } else {
            if (print_xmp_ns) {
                print_all_xmp_namespaces(xmp_tags);
            }
            if (print_exif_tags) {
                foreach (var tag in exif_tags) {
                    print_tag(metadata, tag, print_unknown_tags, print_tag_details);
                }
            }
            if (print_iptc_tags) {
                foreach (var tag in iptc_tags) {
                    print_tag(metadata, tag, print_unknown_tags, print_tag_details);
                }
            }
            if (print_xmp_tags) {
                foreach (var tag in xmp_tags) {
                    print_tag(metadata, tag, print_unknown_tags, print_tag_details);
                }
            }
        }
    } catch (OptionError e) {
        printerr("%s error: %s\n", program_name, e.message);
        print(opt_context.get_help(true, null));
        return 1;
    } catch (Error err) {
        printerr("%s error: Processing metadata for '%s': %s (%i)\n",
                 program_name,
                 args[1],
                 err.message,
                 err.code);
        return 2;
    }

    return 0;
}

/**
 * print_tag:
 * @metadata: An instance of #GExiv2Metadata
 * @tag: The tag to print out
 * @print_unknown_tags: Whether to include unknown tags in the output
 * @print_details: Whether to print the tag details in addition to the tag value
 *
 * Prints out either the tag values or the tag details for the tag. If the tag
 * is unknown (e.g. Exif.Sony2Fp.0x018f) then printing is optionally filtered 
 * out. 
 *
 * Returns:
 */
void print_tag(GExiv2.Metadata metadata, string tag, bool print_unknown_tags, bool print_details) throws Error {

    if (print_unknown_tags || !(".0x" in tag)) {
        if (print_details) {
            print("%-64s%s\n", "Tag", tag);
            print("%-64s%s\n", "  Value", metadata.get_tag_string(tag));
            print("%-64s%s\n", "  Interpreted value", metadata.get_tag_interpreted_string(tag));
            print("%-64s%s\n", "  Label", GExiv2.Metadata.get_tag_label(tag));
            print("%-64s%s\n", "  Type", GExiv2.Metadata.get_tag_type(tag));
            print("%-64s%s\n", "  Description", GExiv2.Metadata.get_tag_description(tag));
        } else {
            print("%-64s%s\n", tag, metadata.get_tag_interpreted_string(tag));
        }
    }
}

/**
 * print_all_xmp_namespaces:
 * @xmp_tags: A list of Xmp tags
 *
 * Prints list of namespaces from Xmp tags with duplicate groupName tags removed.
 * e.g. "Xmp.dc.subject" has the namespace "http://purl.org/dc/elements/1.1/"
 *
 * Returns:
 */
void print_all_xmp_namespaces(string[] xmp_tags) throws Error {

    var last_groupName = ""; // Previous iteration's value

    foreach (var tag in xmp_tags) {

        // Extract groupName from tag (format: "familyName.groupName.tagName")
        var groupName = tag.split(".", 3)[1];

        // Only output one example of a groupName's namespace
        if (last_groupName != groupName) {
            print("Xmp ns: %-56s%s\n", groupName, GExiv2.Metadata.get_xmp_namespace_for_tag(groupName));
            last_groupName = groupName;
        }
    }
}

/**
 * extract_filename:
 * @filename_and_path:
 *
 * Extract the filename part from the path+filename
 * e.g. "/usr/home/username/program" would return "program"
 *      or "C:\temp\program.exe" would return "program.exe"
 *
 * Returns: (transfer full) The filename
 */
string extract_filename(string filename_and_path) {
    var pos = 0;
    for (var i = filename_and_path.length - 1; i >= 0; i--) {
        if (filename_and_path[i] == '/') {
            pos = i + 1;
            break;
        }
        if (filename_and_path[i] == '\\') {
            pos = i + 1;
            break;
        }
    }
    return filename_and_path.substring(pos, filename_and_path.length - pos);
}
