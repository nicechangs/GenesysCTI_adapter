#!/bin/sh

#===============================================================================
#	Genesys Production
#	Copyright (C) 2002-2011 Genesys Telecommunications Laboratories, Inc.
#===============================================================================
#	Description:	Script for performing Genesys products installation
#
#	History of changes:
#		2011-08-11, version 1.15, AlxNik
#			Add info about original launch directory.
#
#		2010-02-16, version 1.14, AlxNik
#			Add PERL5LIB processing.
#
#		2010-01-22, version 1.13, AlxNik
#			Add uninstall data processing.
#
#		2008-09-17, version 1.12, AlxNik
#			Use PATH in comments.
#
#		2008-09-02, version 1.11, AlxNik
#			Merge install_sh_generic_path specific (PATH),
#			merge install_sh_gvp specific (return code).
#
#		2008-07-07, version 1.10, AlxNik
#			Add package tar and release_notes directory support.
#
#		2008-06-09, version 1.09, AlxNik
#			Add genesys_license_agreement.txt support.
#
#		2006-05-30, version 1.08, AlxNik
#			Add genesys_silent.ini support.
#
#		2006-04-26, version 1.07, AlxNik
#			Add silent installation support.
#
#		2005-03-21, version 1.06, AlxNik
#			Update copyright year.
#
#		2004-04-05, version 1.05, AlxNik
#			Add support for restricted releases.
#
#		2003-06-02, version 1.04, AlxNik
#			Add GCTISetup.ini file processing
#
#		2003-03-27, version 1.03, AlxNik
#			Add HUP (1), QUIT (3), TERM (15) signals trapping,
#			copy each file individually
#
#		2003-03-14, version 1.02, AlxNik
#			Ensure that shell_script_name is executable
#
#		2003-03-11, version 1.01, AlxNik
#			Use temporary directory.
#
#		2002-12-12, version 1.00, AlxNik
#			First release
#===============================================================================

#===============================================================================
# do_install function
#===============================================================================

do_install()
{
	PATH=/bin:/usr/bin:$PATH
	export PATH
	PERLLIB=./lib/5.6.1
	export PERLLIB
	unset PERL5LIB
	chmod ug+rwx gunzip install.sh >/dev/null 2>&1
	tar_name=tar
	
	if [ -f tar ]; then
		tar_name=./tar
		chmod ug+rwx tar >/dev/null 2>&1
	fi
	
	./gunzip -c installer.tar.gz | ${tar_name} xf -
	./gunzip -c iscript.tar.gz | ${tar_name} xf -

	do_cleanup()
	{
		./cleanup.sh
		rm ivrapi.pl
		
		if [ -d uninstall ]; then
			rm -rf uninstall
		fi		
	}

	trap "do_cleanup; exit 1" 1 2 3 15

	./Perl ivrapi.pl $* 	
	rc=$?
	
	do_cleanup
	
	return $rc
}

#===============================================================================
# do_dir_cleanup function
#===============================================================================

do_dir_cleanup()
{
	cd /
	rm -rf $1
}

#===============================================================================
# copy_files function
#===============================================================================

copy_files()
{
	ip_description_filename=ip_description.xml
	read_me_filename=read_me.html
	ini_file=GCTISetup.ini
	release_notes_filename=release_notes.html
	silent_mode_response_default_filename=genesys_silent.ini
	genesys_license_agreement_filename=genesys_license_agreement.txt
	release_notes_directory=release_notes
	launch_directory_filename=launch_directory.txt

	cp data.tar.gz $1
	cp gunzip $1
	cp install.sh $1
	cp installer.tar.gz $1
	cp iscript.tar.gz $1
	cp ospatchlist.txt $1

	if [ -f ${ip_description_filename} ]; then
		cp ${ip_description_filename} $1
	fi

	if [ -f ${read_me_filename} ]; then
		cp ${read_me_filename} $1
	fi	

	if [ -f ${ini_file} ]; then
		cp ${ini_file} $1
	fi
	
	if [ -f ${release_notes_filename} ]; then
		cp ${release_notes_filename} $1
	fi
	
	if [ -f ${silent_mode_response_default_filename} ]; then
		cp ${silent_mode_response_default_filename} $1
	fi
	
	if [ -f ${genesys_license_agreement_filename} ]; then
		cp ${genesys_license_agreement_filename} $1
	fi
	
	if [ -f tar ]; then
		cp tar $1
	fi
	
	if [ -f gunzip_user_license_agreement.txt ]; then
		cp gunzip_user_license_agreement.txt $1
	fi
	
	if [ -f tar_gunzip_license.txt ]; then
		cp tar_gunzip_license.txt $1
	fi
	
	if [ -d ${release_notes_directory} ]; then
		cp -r ${release_notes_directory} $1
	fi

	pwd > $1/${launch_directory_filename}
}

#===============================================================================
# define variables
#===============================================================================

current_date=`date '+%Y_%m_%d_%H_%M_%S'`
mark_file=.exists
tmp_dir_name=/tmp/${current_date}_$$

#===============================================================================
# install
#===============================================================================

if [ -f $mark_file ]; then
	do_install $*
	rc=$?
else
	# create dir
	if [ -d ${tmp_dir_name} ]; then
		rm -rf ${tmp_dir_name}
	fi
	
	mkdir ${tmp_dir_name}
	
	trap "do_dir_cleanup ${tmp_dir_name}; exit 1" 1 2 3 15
	
	# copy files
	copy_files ${tmp_dir_name}
	
	# create mark_file
	cd ${tmp_dir_name}
	> ${mark_file}
	
	# execute install
	chmod ug+x install.sh >/dev/null 2>&1	
	${tmp_dir_name}/install.sh $*
	rc=$?
	
	# delete dir
	do_dir_cleanup ${tmp_dir_name}
fi

exit $rc

