/*
 * gexiv2-dump.vala
 *
 * Author(s)
 *  Jim Nelson <jim@yorba.org>
 *
 * This is free software. See COPYING for details.
 */

int main(string[] args) {
    if (args.length != 3 || ("--help" in args) || ("-h" in args)) {
        usage();

        return 1;
    }

    var in_file = File.new_for_commandline_arg (args[1]);
    var out_file = File.new_for_commandline_arg (args[2]);

    var metadata = new GExiv2.Metadata();
    try {
        metadata.from_stream (in_file.read());
    } catch (Error err) {
        print("Failed to open input file %s: %s\n", args[1], err.message);
        return 2;
    }

    print ("Opening destination file for reading");
    try {
        var input = out_file.read();
        var output = out_file.replace(null, false, FileCreateFlags.NONE, null);
        metadata.to_stream (new SimpleIOStream (input, output));
    } catch (Error err) {
        print("Failed write to output file %s: %s\n", args[2], err.message);
        return 3;
    }

    return 0;
}

void usage() {
    stdout.printf("usage: gexiv2-copy IN-FILE OUT-FILE\n\n");
}
