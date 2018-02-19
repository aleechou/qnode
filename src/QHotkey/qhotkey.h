#ifndef QHOTKEY_H
#define QHOTKEY_H

#include <QObject>
#include <QKeySequence>
#include <QPair>
#include <QLoggingCategory>

#ifdef QHOTKEY_LIB
	#ifdef QHOTKEY_LIB_BUILD
		#define QHOTKEY_SHARED_EXPORT Q_DECL_EXPORT
	#else
		#define QHOTKEY_SHARED_EXPORT Q_DECL_IMPORT
	#endif
#else
	#define QHOTKEY_SHARED_EXPORT
#endif

//! A class to define global, systemwide Hotkeys
class QHOTKEY_SHARED_EXPORT QHotkey : public QObject
{
	Q_OBJECT
	friend class QHotkeyPrivate;

	//! Specifies whether this hotkey is currently registered or not
	Q_PROPERTY(bool registered READ isRegistered WRITE setRegistered NOTIFY registeredChanged)
	//! Holds the shortcut this hotkey will be triggered on
    Q_PROPERTY(QKeySequence shortcut READ shortcut WRITE setKeySequenceShortcut RESET resetShortcut)

public:
	//! Defines shortcut with native keycodes
	class QHOTKEY_SHARED_EXPORT NativeShortcut {
	public:
		//! The native keycode
		quint32 key;
		//! The native modifiers
		quint32 modifier;

		//! Creates an invalid native shortcut
		NativeShortcut();
		//! Creates a valid native shortcut, with the given key and modifiers
		NativeShortcut(quint32 key, quint32 modifier = 0);

		//! Checks, whether this shortcut is valid or not
		bool isValid() const;

		//! Equality operator
		bool operator ==(const NativeShortcut &other) const;
		//! Inequality operator
		bool operator !=(const NativeShortcut &other) const;

	private:
		bool valid;
	};

	static void addGlobalMapping(const QKeySequence &shortcut, const NativeShortcut &nativeShortcut);

	//! Constructor
    Q_INVOKABLE explicit QHotkey(QObject *parent = Q_NULLPTR);
	//! Constructs a hotkey with a shortcut and optionally registers it
	explicit QHotkey(const QKeySequence &shortcut, bool autoRegister = false, QObject *parent = Q_NULLPTR);
	//! Constructs a hotkey with a key and modifiers and optionally registers it
	explicit QHotkey(Qt::Key keyCode, Qt::KeyboardModifiers modifiers, bool autoRegister = false, QObject *parent = Q_NULLPTR);
	//! Constructs a hotkey from a native shortcut and optionally registers it
	explicit QHotkey(const NativeShortcut &shortcut, bool autoRegister = false, QObject *parent = Q_NULLPTR);
	//! Destructor
	~QHotkey();

	//! READ-Accessor for QHotkey::registered
    Q_INVOKABLE bool isRegistered() const;
	//! READ-Accessor for QHotkey::shortcut - the key and modifiers as a QKeySequence
    Q_INVOKABLE QString shortcut() const;
	//! READ-Accessor for QHotkey::shortcut - the key only
    Q_INVOKABLE unsigned long keyCode() const;
	//! READ-Accessor for QHotkey::shortcut - the modifiers only
    Q_INVOKABLE unsigned long modifiers() const;

	//! Get the current native shortcut
	NativeShortcut currentNativeShortcut() const;

    //! WRITE-Accessor for QHotkey::shortcut
    bool setKeySequenceShortcut(const QKeySequence &shortcut, bool autoRegister = false);
    //! WRITE-Accessor for QHotkey::shortcut
    bool setKeyShortcut(Qt::Key keyCode, Qt::KeyboardModifiers modifiers, bool autoRegister = false);


public slots:
	//! WRITE-Accessor for QHotkey::registered
	bool setRegistered(bool registered);

    bool setShortcut(const QString &shortcut);

	//! RESET-Accessor for QHotkey::shortcut
	bool resetShortcut();

	//! Set this hotkey to a native shortcut
	bool setNativeShortcut(NativeShortcut nativeShortcut, bool autoRegister = false);


signals:
	//! Will be emitted if the shortcut is pressed
    void activated();

	//! NOTIFY-Accessor for QHotkey::registered
	void registeredChanged(bool registered);

private:
	Qt::Key _keyCode;
	Qt::KeyboardModifiers _modifiers;

	NativeShortcut _nativeShortcut;
	bool _registered;
};

uint QHOTKEY_SHARED_EXPORT qHash(const QHotkey::NativeShortcut &key);
uint QHOTKEY_SHARED_EXPORT qHash(const QHotkey::NativeShortcut &key, uint seed);

QHOTKEY_SHARED_EXPORT Q_DECLARE_LOGGING_CATEGORY(logQHotkey)

Q_DECLARE_METATYPE(QHotkey::NativeShortcut)

#endif // QHOTKEY_H
