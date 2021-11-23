#!/bin/sh

music_directory=""


install_samba()
{
        apt install samba smbclient -y
}

get_mpd_music_dir()
{
        if [ -f /etc/mpd.conf ]; then
                music_directory=`grep "^music_directory" /etc/mpd.conf|awk '{ print $2 }'`
                if [ -n $music_directory -a -d $music_directory ]; then
                        chmod  a+rxw $music_directory
                fi
                return 0
        else
                echo "Get mpd configure file error!"
                exit 0
        fi
}

configure_samba_share()
{

        count=`grep "\[Music\]" /etc/samba/smb.conf|wc -l`
        if [ $count -gt 0 ]; then
                return 1
        fi
        music_dir=`echo $music_directory| sed 's/\"//g'`
        echo "change permission for: \033[1;33m $music_dir \033[0m "
        chmod a+rwx $music_dir
        echo ""  >>/etc/samba/smb.conf
        echo "[Music]"           >>/etc/samba/smb.conf
        echo "   comment = public anonymous access read-write">>/etc/samba/smb.conf
        echo "   path = $music_dir">>/etc/samba/smb.conf
        echo "   browsable = yes">>/etc/samba/smb.conf
        echo "   guest ok = yes">>/etc/samba/smb.conf
        echo "   writable = yes">>/etc/samba/smb.conf
        echo "   create mask = 0644">>/etc/samba/smb.conf
        echo "   directory mode = 0755">>/etc/samba/smb.conf
        echo "">>/etc/samba/smb.conf
}

install_samba
get_mpd_music_dir
configure_samba_share
systemctl restart smbd
echo "Linux samba configure OK..."
echo "Windows 10 , need: regedit modify"

apt-get install python3 -y
cd /opt
git clone https://github.com/christgau/wsdd
cp /opt/wsdd/etc/systemd/wsdd.service  /etc/systemd/system/
cp /opt/wsdd/src/wsdd.py /usr/local/bin/wsdd.py
chmod a+x /usr/local/bin/wsdd.py
ln -s /usr/local/bin/wsdd.py /usr/bin/wsdd
systemctl daemon-reload
systemctl enable wsdd.service
systemctl start wsdd
