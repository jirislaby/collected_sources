tree grammar TWalker;
options {
	tokenVocab=T;
	ASTLabelType=CommonTree;
}
@header {
import java.util.HashMap;
import java.util.Map;
}
@members {
	private Map<String, Ident> ids = new HashMap<String, Ident>();

	private class Ident<T> {
		int type;
		T val;
		Ident(int type) {
			this.type = type;
		}
		Ident(int type, T val) {
			this.type = type;
			this.val = val;
		}
		int getType() {
			return type;
		}
		void set(Ident val) {
			if (val.getType() == 1)
				this.val = (T)(Integer)val.get();
			else if (val.getType() == 2)
				this.val = (T)(Float)val.get();
		}
		T get() {
			return val;
		}
		Ident add(Ident id) {
			Ident i;
			if (id.getType() == 2 || type == 2)
				i = new Ident<Float>(2, (Float)id.val +
						(Float)val);
			else
				i = new Ident<Integer>(1, (Integer)id.val +
						(Integer)val);
			return i;
		}
		Ident sub(Ident id) {
			Ident i;
			if (id.getType() == 2 || type == 2)
				i = new Ident<Float>(2, (Float)val -
						(Float)id.val);
			else
				i = new Ident<Integer>(1, (Integer)val -
						(Integer)id.val);
			return i;
		}
		public int hashCode() {
			return val.hashCode();
		}
		public String toString() {
			return val.toString();
		}
	};

	private void addId(String name, int type) {
		if (ids.containsKey(name)) {
			System.out.println("warning: redefined variable '" +
					name + "'. Ignoring.");
			return;
		}
		Ident id = null; /* java is buggy too */
		if (type == 1)
			id = new Ident<Integer>(1);
		else if (type == 2)
			id = new Ident<Float>(2);
		else {
			System.out.println("compiler bug");
			System.exit(1);
		}
		ids.put(name, id);
		System.out.println(name + " added (" + type + "): " + id.getType());
	}
	private Ident getId(String name) {
		if (!ids.containsKey(name)) {
			System.out.println("undefined reference to '" + name +
					"'");
			return null;
		}
		return ids.get(name);
	}
	private void setId(String name, Ident id) {
		if (!ids.containsKey(name)) {
			System.out.println("undefined reference to '" + name +
					"'");
			return;
		}
		ids.get(name).set(id);
	}
}

root	: ^(BLOCK def* line*)
	;

def	: ^(VARDEF type ID)	{ addId($ID.text, $type.type); }
	;

line	: ^('=' ID expr)	{ setId($ID.text, $expr.val); }
	| ^('print' expr)	{ System.out.println($expr.val + " " + $expr.val.getType()); }
	;

expr returns [Ident val]
	: ^('+' a=expr b=expr)	{ $val = $a.val.add($b.val); }
	| ^('-' a=expr b=expr)	{ $val = $a.val.sub($b.val); }
	| ^('*' a=expr b=expr)	{ /*$val = $a.val * $b.val;*/ }
	| ^('/' a=expr b=expr)	{ /*$val = $a.val / $b.val;*/ }
	| ID			{ $val = getId($ID.text); }
	| INT			{ $val = new Ident<Integer>(1, Integer.valueOf(
					$INT.text)); }
	| REAL			{ $val = new Ident<Float>(2, Float.valueOf(
					$REAL.text)); }
	;

type returns [int type]
	: 'int'		{ $type = 1; }
	| 'real'	{ $type = 2; }
	;

