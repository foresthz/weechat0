#
# Copyright (c) 2008-2009 by FlashCode <flashcode@flashtux.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# Documentation generator for WeeChat: build XML include files with commands,
# options, infos and completions for WeeChat core and plugins.
#
# Instructions to build config files yourself in WeeChat directories (replace
# all paths with your path to WeeChat):
#     1.  run WeeChat and load this script, with following command:
#           /perl load ~/src/weechat/doc/docgen.pl
#     2.  change path to build in your doc/ directory:
#           /set plugins.var.perl.docgen.path "~/src/weechat/doc"
#     3.  run docgen command:
#           /docgen
# XML files should be in ~/src/weechat/doc/xx/autogen/ (where xx is language)
#
# Script written on 2008-08-22 by FlashCode <flashcode@flashtux.org>
#

use strict;

use POSIX;            # needed for setlocale()
use Locale::gettext;
use File::Basename;

my $version = "0.1";

# -------------------------------[ config ]------------------------------------

# default path where doc XML files will be written (should be doc/ in sources
# package tree)
# path must have subdirectories with languages and autogen directory:
#      path
#       |-- en
#       |   |-- autogen
#       |-- fr
#       |   |-- autogen
#       ...
my $default_path = "~/src/weechat/doc";

# list of locales for which we want to build XML doc files to include
my @locale_list = qw(en_US fr_FR de_DE);

# all commands/options/.. of following plugins will produce a file
# non-listed plugins will be ignored
# value: "c" = plugin may have many commands
#        "o" = write config options for plugin
# if plugin is listed without "c", that means plugin has only one command
# /name (where "name" # is name of plugin)
# Note: we consider core is a plugin called "weechat"
my %plugin_list = ("weechat" => "co", "alias"   => "",
                   "aspell"  => "o",  "charset" => "co",
                   "demo"    => "co", "fifo"    => "co",
                   "irc"     => "co", "jabber"  => "co",
                   "logger"  => "co", "relay"   => "co",
                   "perl"    => "",   "python"  => "",
                   "ruby"    => "",   "lua"     => "",
                   "tcl"     => "",   "xfer"    => "co");

# options to ignore
my @ignore_options = ("aspell\\.dict\\..*",
                      "charset\\.decode\\..*",
                      "charset\\.encode\\..*",
                      "irc\\.server\\..*",
                      "jabber\\.server\\..*",
                      "logger\\.level\\..*",
                      "logger\\.mask\\..*",
                      "weechat\\.proxy\\..*",
                      "weechat\\.bar\\..*",
                      "weechat\\.debug\\..*",
                      "weechat\\.notify\\..*");

# infos to ignore
my @ignore_infos_plugins = ("jabber");

# infolists to ignore
my @ignore_infolists_plugins = ("jabber");

# completions to ignore
my @ignore_completions_plugins = ("jabber");
my @ignore_completions_items = ("jabber.*",
                                "weeget.*");

# -------------------------------[ init ]--------------------------------------

weechat::register("docgen", "FlashCode <flashcode\@flashtux.org>", $version,
                  "GPL", "Doc generator for WeeChat 0.2.7", "", "");
weechat::hook_command("docgen", "Doc generator", "", "", "", "docgen", "");
weechat::config_set_plugin("path", $default_path)
    if (weechat::config_get_plugin("path") eq "");

# -----------------------------------------------------------------------------

# get list of commands in a hash with 3 indexes: plugin, command, xxx
sub get_commands
{
    my %commands;
    
    my $infolist = weechat::infolist_get("hook", "", "command");
    while (weechat::infolist_next($infolist))
    {
        my $command = weechat::infolist_string($infolist, "command");
        my $plugin = weechat::infolist_string($infolist, "plugin_name");
        $plugin = "weechat" if ($plugin eq "");
        if (exists($plugin_list{$plugin}))
        {
            if (($command eq $plugin) || ($plugin_list{$plugin} =~ /c/))
            {
                $commands{$plugin}{$command}{"description"} = weechat::infolist_string($infolist, "description");
                $commands{$plugin}{$command}{"args"} = weechat::infolist_string($infolist, "args");
                $commands{$plugin}{$command}{"args_description"} = weechat::infolist_string($infolist, "args_description");
                $commands{$plugin}{$command}{"completion"} = weechat::infolist_string($infolist, "completion");
            }
        }
    }
    weechat::infolist_free($infolist);
    
    return %commands;
}

# get list of config options in a hash with 4 indexes: config, section, option, xxx
sub get_options
{
    my %options;
    
    my $infolist = weechat::infolist_get("option", "", "");
    while (weechat::infolist_next($infolist))
    {
        my $full_name = weechat::infolist_string($infolist, "full_name");
        
        # check if option is ignored or not
        my $ignore = 0;
        foreach my $mask (@ignore_options)
        {
            $ignore = 1 if ($full_name =~ /${mask}/);
        }
        
        if ($ignore ne 1)
        {
            my $config = weechat::infolist_string($infolist, "config_name");
            my $section = weechat::infolist_string($infolist, "section_name");
            my $option = weechat::infolist_string($infolist, "option_name");
            if (defined $plugin_list{$config} && ($plugin_list{$config} =~ /o/))
            {
                $options{$config}{$section}{$option}{"type"} = weechat::infolist_string($infolist, "type");
                $options{$config}{$section}{$option}{"string_values"} = weechat::infolist_string($infolist, "string_values");
                $options{$config}{$section}{$option}{"default_value"} = weechat::infolist_string($infolist, "default_value");
                $options{$config}{$section}{$option}{"min"} = weechat::infolist_integer($infolist, "min");
                $options{$config}{$section}{$option}{"max"} = weechat::infolist_integer($infolist, "max");
                $options{$config}{$section}{$option}{"description"} = weechat::infolist_string($infolist, "description");
            }
        }
    }
    weechat::infolist_free($infolist);
    
    return %options;
}

# get list of infos hooked by plugins in a hash with 3 indexes: plugin, name, xxx
sub get_infos
{
    my %infos;
    
    # get infos hooked
    my $infolist = weechat::infolist_get("hook", "", "info");
    while (weechat::infolist_next($infolist))
    {
        my $info_name = weechat::infolist_string($infolist, "info_name");
        my $plugin = weechat::infolist_string($infolist, "plugin_name");
        $plugin = "weechat" if ($plugin eq "");
        
        # check if infolist is ignored or not
        my $ignore = 0;
        foreach my $mask (@ignore_infos_plugins)
        {
            $ignore = 1 if ($plugin =~ /${mask}/);
        }
        
        if ($ignore ne 1)
        {
            $infos{$plugin}{$info_name}{"description"} = weechat::infolist_string($infolist, "description");
        }
    }
    weechat::infolist_free($infolist);
    
    return %infos;
}

# get list of infolists hooked by plugins in a hash with 3 indexes: plugin, name, xxx
sub get_infolists
{
    my %infolists;
    
    # get infolists hooked
    my $infolist = weechat::infolist_get("hook", "", "infolist");
    while (weechat::infolist_next($infolist))
    {
        my $infolist_name = weechat::infolist_string($infolist, "infolist_name");
        my $plugin = weechat::infolist_string($infolist, "plugin_name");
        $plugin = "weechat" if ($plugin eq "");
        
        # check if infolist is ignored or not
        my $ignore = 0;
        foreach my $mask (@ignore_infolists_plugins)
        {
            $ignore = 1 if ($plugin =~ /${mask}/);
        }
        
        if ($ignore ne 1)
        {
            $infolists{$plugin}{$infolist_name}{"description"} = weechat::infolist_string($infolist, "description");
        }
    }
    weechat::infolist_free($infolist);
    
    return %infolists;
}

# get list of completions hooked by plugins in a hash with 3 indexes: plugin, item, xxx
sub get_completions
{
    my %completions;
    
    # get completions hooked
    my $infolist = weechat::infolist_get("hook", "", "completion");
    while (weechat::infolist_next($infolist))
    {
        my $completion_item = weechat::infolist_string($infolist, "completion_item");
        my $plugin = weechat::infolist_string($infolist, "plugin_name");
        $plugin = "weechat" if ($plugin eq "");
        
        # check if completion item is ignored or not
        my $ignore = 0;
        foreach my $mask (@ignore_completions_plugins)
        {
            $ignore = 1 if ($plugin =~ /${mask}/);
        }
        foreach my $mask (@ignore_completions_items)
        {
            $ignore = 1 if ($completion_item =~ /${mask}/);
        }
        
        if ($ignore ne 1)
        {
            $completions{$plugin}{$completion_item}{"description"} = weechat::infolist_string($infolist, "description");
        }
    }
    weechat::infolist_free($infolist);
    
    return %completions;
}

# escape string for XML output
sub escape
{
    my $str = $_[0];
    $str =~ s/</&lt;/g;
    $str =~ s/>/&gt;/g;
    return $str;
}

# build XML doc files (command /docgen)
sub docgen
{
    my %plugin_commands = get_commands();
    my %plugin_options = get_options();
    my %plugin_infos = get_infos();
    my %plugin_infolists = get_infolists();
    my %plugin_completions = get_completions();
    
    # xml header (comment) for all files
    my $xml_header =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        ."<!--\n"
        ."     ********* WARNING! *********\n"
        ."\n"
        ."     This file is autogenerated with docgen.pl script. *** DO NOT EDIT! ***\n"
        ."     docgen.pl builds XML doc files to include in many languages\n"
        ."-->\n\n";
    
    # get path and replace ~ by home if needed
    my $path = weechat::config_get_plugin("path");
    $path =~ s/^~\//$ENV{"HOME"}\//;
    
    my $old_locale = setlocale(LC_MESSAGES);
    
    # write to doc files, by locale
    my $num_files = 0;
    my $num_files_updated = 0;
    my $filename = "";
    foreach my $locale (@locale_list)
    {
        my $num_files_commands = 0;
        my $num_files_commands_updated = 0;
        my $num_files_options = 0;
        my $num_files_options_updated = 0;
        my $num_files_infos = 0;
        my $num_files_infos_updated = 0;
        my $num_files_infolists = 0;
        my $num_files_infolists_updated = 0;
        my $num_files_completions = 0;
        my $num_files_completions_updated = 0;
        
        setlocale(LC_MESSAGES, $locale.".UTF-8");
        my $d = Locale::gettext->domain_raw("weechat");
        $d->codeset("UTF-8");
        $d->dir(weechat::info_get("weechat_localedir", ""));
        
        my $dir = $path."/".substr($locale, 0, 2)."/autogen/";
        if (-d $dir)
        {
            # write commands
            foreach my $plugin (keys %plugin_commands)
            {
                $filename = $dir.$plugin."_commands.xml";
                if (open(FILE, ">".$filename.".tmp"))
                {
                    print FILE $xml_header;
                    foreach my $command (sort keys %{$plugin_commands{$plugin}})
                    {
                        my $args = $plugin_commands{$plugin}{$command}{"args"};
                        $args = $d->get($args) if ($args ne "");
                        my $description = $plugin_commands{$plugin}{$command}{"description"};
                        $description = $d->get($description) if ($description ne "");
                        my $args_description = $plugin_commands{$plugin}{$command}{"args_description"};
                        $args_description = $d->get($args_description) if ($args_description ne "");
                        
                        print FILE "<command>/".$command;
                        print FILE escape("  ".$args) if ($args ne "");
                        print FILE "</command>\n";
                        print FILE "<programlisting>\n";
                        print FILE escape($description."\n") if ($description ne "");
                        print FILE escape("\n".$args_description."\n") if ($args_description ne "");
                        print FILE "</programlisting>\n\n";
                    }
                    #weechat::print("", "docgen: file ok: '$filename'");
                    my $rc = system("diff ".$filename." ".$filename.".tmp >/dev/null 2>&1");
                    if ($rc != 0)
                    {
                        system("mv -f ".$filename.".tmp ".$filename);
                        $num_files_updated++;
                        $num_files_commands_updated++;
                    }
                    else
                    {
                        system("rm ".$filename.".tmp");
                    }
                    $num_files++;
                    $num_files_commands++;
                    close(FILE);
                }
                else
                {
                    weechat::print("", weechat::prefix("error")."docgen error: unable to write file '$filename'");
                }
            }
            
            # write config options
            foreach my $config (keys %plugin_options)
            {
                $filename = $dir.$config."_options.xml";
                if (open(FILE, ">".$filename.".tmp"))
                {
                    print FILE $xml_header;
                    foreach my $section (sort keys %{$plugin_options{$config}})
                    {
                        foreach my $option (sort keys %{$plugin_options{$config}{$section}})
                        {
                            my $type = $plugin_options{$config}{$section}{$option}{"type"};
                            my $string_values = $plugin_options{$config}{$section}{$option}{"string_values"};
                            my $default_value = $plugin_options{$config}{$section}{$option}{"default_value"};
                            my $min = $plugin_options{$config}{$section}{$option}{"min"};
                            my $max = $plugin_options{$config}{$section}{$option}{"max"};
                            my $description = $plugin_options{$config}{$section}{$option}{"description"};
                            $description = $d->get($description) if ($description ne "");
                            my $type_nls = $type;
                            $type_nls = $d->get($type_nls) if ($type_nls ne "");
                            my $values = "";
                            if ($type eq "boolean")
                            {
                                $values = "on, off";
                            }
                            if ($type eq "integer")
                            {
                                if ($string_values ne "")
                                {
                                    $string_values =~ s/\|/, /g;
                                    $values = $string_values;
                                }
                                else
                                {
                                    $values = $min." .. ".$max;
                                }
                            }
                            if ($type eq "string")
                            {
                                $values = $d->get("any string") if ($max <= 0);
                                $values = $d->get("any char") if ($max == 1);
                                $values = $d->get("any string")." (".$d->get("max chars").": ".$max.")" if ($max > 1);
                                $default_value = "'".$default_value."'";
                            }
                            if ($type eq "color")
                            {
                                $values = $d->get("a color name");
                            }
                            
                            print FILE "<command>".$config.".".$section.".".$option."</command>: ".escape($description)."\n";
                            print FILE "<itemizedlist>\n";
                            print FILE "  <listitem>\n";
                            print FILE "    <para>".$d->get("type").": ".$type_nls."</para>\n";
                            print FILE "  </listitem>\n";
                            print FILE "  <listitem>\n";
                            print FILE "    <para>".$d->get("values").": ".escape($values)." "
                                ."(".$d->get("default value").": ".escape($default_value).")</para>\n";
                            print FILE "  </listitem>\n";
                            print FILE "</itemizedlist>\n\n";
                        }
                    }
                    #weechat::print("", "docgen: file ok: '$filename'");
                    my $rc = system("diff ".$filename." ".$filename.".tmp >/dev/null 2>&1");
                    if ($rc != 0)
                    {
                        system("mv -f ".$filename.".tmp ".$filename);
                        $num_files_updated++;
                        $num_files_options_updated++;
                    }
                    else
                    {
                        system("rm ".$filename.".tmp");
                    }
                    $num_files++;
                    $num_files_options++;
                    close(FILE);
                }
                else
                {
                    weechat::print("", weechat::prefix("error")."docgen error: unable to write file '$filename'");
                }
            }
            
            # write infos hooked
            $filename = $dir."infos.xml";
            if (open(FILE, ">".$filename.".tmp"))
            {
                print FILE $xml_header;
                foreach my $plugin (sort keys %plugin_infos)
                {
                    foreach my $info (sort keys %{$plugin_infos{$plugin}})
                    {
                        my $description = $plugin_infos{$plugin}{$info}{"description"};
                        $description = $d->get($description) if ($description ne "");
                        
                        print FILE "<row>\n";
                        print FILE "  <entry>".escape($plugin)."</entry>\n";
                        print FILE "  <entry>".escape($info)."</entry>\n";
                        print FILE "  <entry>".escape($description)."</entry>\n";
                        print FILE "</row>\n";
                    }
                }
                #weechat::print("", "docgen: file ok: '$filename'");
                my $rc = system("diff ".$filename." ".$filename.".tmp >/dev/null 2>&1");
                if ($rc != 0)
                {
                    system("mv -f ".$filename.".tmp ".$filename);
                    $num_files_updated++;
                    $num_files_infos_updated++;
                }
                else
                {
                    system("rm ".$filename.".tmp");
                }
                $num_files++;
                $num_files_infos++;
                close(FILE);
            }
            else
            {
                weechat::print("", weechat::prefix("error")."docgen error: unable to write file '$filename'");
            }
            
            # write infolists hooked
            $filename = $dir."infolists.xml";
            if (open(FILE, ">".$filename.".tmp"))
            {
                print FILE $xml_header;
                foreach my $plugin (sort keys %plugin_infolists)
                {
                    foreach my $infolist (sort keys %{$plugin_infolists{$plugin}})
                    {
                        my $description = $plugin_infolists{$plugin}{$infolist}{"description"};
                        $description = $d->get($description) if ($description ne "");
                        
                        print FILE "<row>\n";
                        print FILE "  <entry>".escape($plugin)."</entry>\n";
                        print FILE "  <entry>".escape($infolist)."</entry>\n";
                        print FILE "  <entry>".escape($description)."</entry>\n";
                        print FILE "</row>\n";
                    }
                }
                #weechat::print("", "docgen: file ok: '$filename'");
                my $rc = system("diff ".$filename." ".$filename.".tmp >/dev/null 2>&1");
                if ($rc != 0)
                {
                    system("mv -f ".$filename.".tmp ".$filename);
                    $num_files_updated++;
                    $num_files_infolists_updated++;
                }
                else
                {
                    system("rm ".$filename.".tmp");
                }
                $num_files++;
                $num_files_infolists++;
                close(FILE);
            }
            else
            {
                weechat::print("", weechat::prefix("error")."docgen error: unable to write file '$filename'");
            }
            
            # write completions hooked
            $filename = $dir."completions.xml";
            if (open(FILE, ">".$filename.".tmp"))
            {
                print FILE $xml_header;
                foreach my $plugin (sort keys %plugin_completions)
                {
                    foreach my $completion_item (sort keys %{$plugin_completions{$plugin}})
                    {
                        my $description = $plugin_completions{$plugin}{$completion_item}{"description"};
                        $description = $d->get($description) if ($description ne "");
                        
                        print FILE "<row>\n";
                        print FILE "  <entry>".escape($plugin)."</entry>\n";
                        print FILE "  <entry>".escape($completion_item)."</entry>\n";
                        print FILE "  <entry>".escape($description)."</entry>\n";
                        print FILE "</row>\n";
                    }
                }
                #weechat::print("", "docgen: file ok: '$filename'");
                my $rc = system("diff ".$filename." ".$filename.".tmp >/dev/null 2>&1");
                if ($rc != 0)
                {
                    system("mv -f ".$filename.".tmp ".$filename);
                    $num_files_updated++;
                    $num_files_completions_updated++;
                }
                else
                {
                    system("rm ".$filename.".tmp");
                }
                $num_files++;
                $num_files_completions++;
                close(FILE);
            }
            else
            {
                weechat::print("", weechat::prefix("error")."docgen error: unable to write file '$filename'");
            }
        }
        else
        {
            weechat::print("", weechat::prefix("error")."docgen error: directory '$dir' does not exist");
        }
        my $total_files = $num_files_commands + $num_files_options
            + $num_files_infos + $num_files_infolists + $num_files_completions;
        my $total_files_updated = $num_files_commands_updated
            + $num_files_options_updated + $num_files_infos_updated
            + $num_files_infolists_updated + $num_files_completions_updated;
        weechat::print("", "docgen: ".$locale.": ".$total_files." files ("
                       .$num_files_commands." cmd, "
                       .$num_files_options." opt, "
                       .$num_files_infos." infos, "
                       .$num_files_infolists." infolists, "
                       .$num_files_completions." complt) -- "
                       .$total_files_updated." updated ("
                       .$num_files_commands_updated." cmd, "
                       .$num_files_options_updated." opt, "
                       .$num_files_infos_updated." infos, "
                       .$num_files_infolists_updated." infolists, "
                       .$num_files_completions_updated." complt)");
    }
    weechat::print("", "docgen: total: ".$num_files." files (".$num_files_updated." updated)");
    
    # write "include_autogen.xml" file (with includes for all files built)
    if ($num_files > 0)
    {
        $filename = $path."/include_autogen.xml";
        if (open(FILE, ">".$filename.".tmp"))
        {
            print FILE "<!-- commands -->\n\n";
            foreach my $plugin (sort keys %plugin_commands)
            {
                print FILE "<!ENTITY ".$plugin."_commands.xml SYSTEM \"autogen/".$plugin."_commands.xml\">\n";
            }
            print FILE "\n<!-- config options -->\n\n";
            foreach my $config (sort keys %plugin_options)
            {
                print FILE "<!ENTITY ".$config."_options.xml SYSTEM \"autogen/".$config."_options.xml\">\n";
            }
            print FILE "\n<!-- infos hooked -->\n\n";
            print FILE "<!ENTITY infos.xml SYSTEM \"autogen/infos.xml\">\n";
            print FILE "\n<!-- infolists hooked -->\n\n";
            print FILE "<!ENTITY infolists.xml SYSTEM \"autogen/infolists.xml\">\n";
            print FILE "\n<!-- completions hooked -->\n\n";
            print FILE "<!ENTITY completions.xml SYSTEM \"autogen/completions.xml\">\n";
            close(FILE);
            my $rc = system("diff ".$filename." ".$filename.".tmp >/dev/null 2>&1");
            if ($rc != 0)
            {
                weechat::print("", "docgen: file ".basename($filename)." updated");
                system("mv -f ".$filename.".tmp ".$filename);
            }
            else
            {
                system("rm ".$filename.".tmp");
            }
        }
        else
        {
            weechat::print("", weechat::prefix("error")."docgen error: unable to write file '$filename'");
        }
    }
    
    setlocale(LC_MESSAGES, $old_locale);
    
    return weechat::WEECHAT_RC_OK;
}